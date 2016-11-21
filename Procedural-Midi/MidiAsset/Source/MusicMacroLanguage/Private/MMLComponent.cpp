// Copyright -> Scott Bishel

#include "MusicMacroLanguagePrivatePCH.h"
#include "MMLComponent.h"

const uint16_t notefreq[] = {
	32,    34,    36,    38,    41,    43,    46,    48,    51,    55,    58,    61,
	65,    69,    73,    77,    82,    87,    92,    97,   103,   110,   116,   123,
	130,   138,   146,   155,   164,   174,   184,   195,   207,   220,   233,   246,
	261,   277,   293,   311,   329,   349,   369,   391,   415,   440,   466,   493,
	523,   554,   587,   622,   659,   698,   739,   783,   830,   880,   932,   987,
	1046,  1108,  1174,  1244,  1318,  1396,  1479,  1567,  1661,  1760,  1864,  1975,
	2093,  2217,  2349,  2489,  2637,  2793,  2959,  3135,  3322,  3520,  3729,  3951,
	4186,  4434,  4698,  4978,  5274,  5587,  5919,  6271,  6644,  7040,  7458,  7902,
	8372,  8869,  9397,  9956, 10548, 11175, 11839, 12543, 13289, 14080, 14917, 15804,
	16744, 17739, 18794, 19912, 21096, 22350, 23679, 25087, 26579, 28160, 29834, 31608,
	33488, 35479, 37589, 39824, 42192, 44701, 47359, 50175,
};

static uint32_t get_note_length_ms(CHORD *p, uint32_t note_ticks)
{
	return (60000) * note_ticks / p->bpm / p->bticks;
}

static void note_clear(CHORD *p)
{
	p->count = 0;
}

static void note_stack(CHORD *p, int number)
{
	uint16_t freq = notefreq[number];
	if ((p->count + 1) < CHORD_MAX_NOTES) {
		p->freqlist[p->count++] = freq;
	}
}

static void chord_init(CHORD *p, int bpm, int bticks)
{
	p->bpm = bpm;
	p->bticks = bticks;
}

static void note_sound(CHORD *p, int ticks, UMMLComponent* component)
{
	uint32_t ms = get_note_length_ms(p, ticks);
	uint32_t cnt_ms = 0;
	while (1) {
		uint8_t i;
		for (i = 0; i < p->count; i++) {
			component->OnNote.Broadcast(p->freqlist[i]);
			component->OnRest.Broadcast(CHORD_SPLIT_TIME_MS);
			cnt_ms += CHORD_SPLIT_TIME_MS;
			if (ms <= cnt_ms) {
				return;
			}
		}
		component->OnRest.Broadcast(CHORD_SPLIT_TIME_MS);
		cnt_ms += CHORD_SPLIT_TIME_MS;
		if (ms <= cnt_ms) {
			return;
		}
	}
}

static void rest_sound(CHORD *p, int ticks, UMMLComponent* component)
{
	uint32_t ms = get_note_length_ms(p, ticks);
	component->OnRest.Broadcast(ms);
}

static void mml_callback(MML_INFO *p, void *extobj)
{
	UMMLComponent* component = (UMMLComponent*)extobj;
	CHORD& chord = component->chord;
	switch (p->type) {
	case MML_TYPE_TEMPO:
	{
		MML_ARGS_TEMPO *args = &(p->args.tempo);
		chord_init(&chord, args->value, component->mml_opt.bticks);
	}
	break;
	case MML_TYPE_NOTE:
	{
		MML_ARGS_NOTE *args = &(p->args.note);
		note_stack(&chord, args->number);

		if (0 < args->ticks) {
			note_sound(&chord, args->ticks, component);
			note_clear(&chord);
		}
	}
	break;
	case MML_TYPE_REST:
	{
		MML_ARGS_REST *args = &(p->args.rest);
		rest_sound(&chord, args->ticks, component);
	}
	break;
	}
}


// Sets default values for this component's properties
UMMLComponent::UMMLComponent() : isLoaded(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.TickInterval = 0.01f;

	/*
	* Initialize the MML module.
	*/
	mml_init(&mml, mml_callback, this);
	MML_OPTION_INITIALIZER_DEFAULT(&mml_opt);

	/*
	* Initialize the chord module.
	*/
	int tempo_default = 120;
	chord_init(&chord, tempo_default, mml_opt.bticks);
}

UMMLComponent::~UMMLComponent() {
	delete[] MMLDataChar;
}


// Called when the game starts
void UMMLComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}


// Called every frame
void UMMLComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(isLoaded)
		mml_fetch(&mml);
}

// Called when the game starts
void UMMLComponent::LoadString(const FString &data)
{
	isLoaded = false;

	TArray<TCHAR> CharData = data.GetCharArray();
	delete[] MMLDataChar;
	MMLDataChar = new char[CharData.Num()];
	for (int i = 0; i < CharData.Num(); i++)
		MMLDataChar[i] = CharData[i];
	mml_setup(&mml, &mml_opt, (char*)MMLDataChar);
	isLoaded = true;
}
