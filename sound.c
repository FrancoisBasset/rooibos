#include <alsa/asoundlib.h>
#include "sound.h"
#include "objects.h"

static int success;
static long max;
static snd_mixer_t *handle;
static snd_mixer_elem_t *elem;
static snd_mixer_selem_id_t *sid;

void sound_init(void) {
	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, "default");
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);
	snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");
    elem = snd_mixer_find_selem(handle, sid);

    if (!elem) {
        snd_mixer_close(handle);
		success = 0;
    } else {
		success = 1;
	}
}

int sound_get_volume(void) {
	long volume = 0;
	long min;

	if (success == 1) {
		snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    	snd_mixer_selem_get_playback_volume(elem, SND_MIXER_ELEM_SIMPLE, &volume);
	}

	double doubleVolume = ((double) volume / (double) max) * 100;

	if (doubleVolume == 0 || doubleVolume == 100) {
		return (int) doubleVolume;
	}

	if (doubleVolume == (int) doubleVolume) {
		return (int) doubleVolume;
	} else if (doubleVolume - (int) doubleVolume < 0.5) {
		return (int) doubleVolume - 1;
	} else {
		return (int) doubleVolume + 1;
	}
}

static void sound_set_volume(int volume) {
	snd_mixer_selem_set_playback_volume_all(elem, volume);
}

void sound_volume_up() {
	int volume = sound_get_volume();
	if (volume != 100) {
		volume += 5;
	}

	volume = (0.01 * volume) * max;

	sound_set_volume(volume);
}

void sound_volume_down() {
	int volume = sound_get_volume();
	if (volume != 0) {
		volume -= 5;
	}

	volume = (0.01 * volume) * max;

	sound_set_volume(volume);
}

void sound_show(void) {
	int volume = sound_get_volume();
	char *text = malloc(sizeof(char) * 100);
	sprintf(text, "Volume : %d%%", volume);

	XDrawString(display, window, gc_text_white, 10, 10, text, strlen(text));
}
