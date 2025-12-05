# Hardware pour l'audio -- JUNIA

**Intervenant :** Alexis ZBIK\
**Contact :** alexis.zbik@gmail.com

Toute la documentation du **Daisy Seed** se trouve ici :\
https://daisy.audio/

## Installation

Suivre le tutoriel officiel pour installer l'environnement de
développement C++ :\
https://daisy.audio/tutorials/cpp-dev-env/

## Librairie DaisySP

### Liste des objets DaisySP vus en cours

------------------------------------------------------------------------

## Oscillator

Oscillateur simple à forme d'onde fixe.\
Documentation : https://daisy.audio/DaisySP/classdaisysp_1\_1Oscillator/

### Initialiser avec la fréquence d'échantillonnage

``` cpp
osc.Init(sampleRate);
```

### Modifier la waveform

``` cpp
osc.SetWaveform(Oscillator::WAVE_SQUARE);
```

### Modifier la fréquence

``` cpp
osc.SetFreq(freq);
osc.SetFreq(mtof(pitch));
```

### Récupérer le signal de sortie

``` cpp
float out = osc.Process();
```

------------------------------------------------------------------------

## Svf

Filtre multi-sortie (passe-bas, passe-haut, bande, notch...)\
Documentation : https://daisy.audio/DaisySP/classdaisysp_1\_1Svf/

### Initialisation

``` cpp
svf.Init(sampleRate);
```

### Réglages

``` cpp
svf.SetFreq(freq); // Réglage de la fréquence
svf.SetRes(resonance); // Réglage de la résonance de 0 à 1, float
```

### Traitement

``` cpp
svf.Process(input);
float low  = svf.Low();
float high = svf.High();
float band = svf.Band();
```

------------------------------------------------------------------------

## AdEnv

Enveloppe Attack--Decay\
Documentation : https://daisy.audio/DaisySP/classdaisysp_1\_1AdEnv/

### Initialisation

``` cpp
env.Init(sampleRate);
```

### Paramétrage

``` cpp
env.SetTime(ADENV_SEG_ATTACK, attackTime); // Attack, en secondes
env.SetTime(ADENV_SEG_DECAY, decayTime); // Decay, en secondes
env.SetMax(maxLevel);
```

### Déclenchement

``` cpp
env.Trigger();
float envOut = env.Process();
```

------------------------------------------------------------------------

## Exemple : Oscillateur + Enveloppe + Filtre

``` cpp
float ProcessSample()
{
    float amp = env.Process();
    float sig = osc.Process() * amp;

    svf.Process(sig);
    return svf.Low();
}
```
