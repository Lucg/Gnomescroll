#include "wav.hpp"

namespace Sound
{

static WavData* wav_buffers = NULL;
static const char base_path[] = "./media/sound/wav/";

void print_wav_data(WavData* data)
{
    printf(
        "WavData:\n"
        "format: %hd\n"
        "channels: %hd\n"
        "sample rate: %d\n"
        "byte rate: %d\n"
        "bits per sample: %hd\n"
        "duration (seconds): %0.2f\n"
        "size (bytes):  %d\n"
        "in use: %d\n",
        data->format, data->channels, data->sample_rate, data->byte_rate,
        data->bits_per_sample, data->duration, data->size, data->in_use
    );
}

#ifdef USE_OPENAL
ALenum get_openal_wav_format(WavData* data)
{
    if (data->channels == 1)
    {
        if (data->bits_per_sample == 8)
            return AL_FORMAT_MONO8;
        else
            return AL_FORMAT_MONO16;
    }
    else
    {
        if (data->bits_per_sample == 8)
            return AL_FORMAT_STEREO8;
        else
            return AL_FORMAT_STEREO16;
    }

}
#endif

int get_free_wav_data(WavData** data)
{
    *data = NULL;
    int i;
    for (i=0; i<MAX_WAV_BUFFERS; i++)
        if (!wav_buffers[i].in_use)
        {
            *data = &wav_buffers[i];
            break;
        }
    return i;
}

bool is_valid_wav(FILE* f)
{
    char metadata[5] = {'\0'};

    if (fread(metadata, sizeof(char), 4, f) != 4 || strcmp(metadata, "RIFF") != 0)
        return false;

    if (fread(metadata, sizeof(char), 4, f) != 4)   // chunk size, dont need (fixed size)
        return false;

    if (fread(metadata, sizeof(char), 4, f) != 4 || strcmp(metadata, "WAVE") != 0)
        return false;

    return true;
}

bool read_wav_fmt_subchunk(FILE* f, WavData* data)
{
    char metadata[5] = {'\0'};
    int read;
    int total_read = 0;

    read = fread(metadata, sizeof(char), 4, f);
    if (read != 4)
        return false;
    if (strcmp(metadata, "fmt ") != 0)
        return false;
        
    read = fread(metadata, sizeof(char), 4, f);
    if (read != 4)
        return false;
    int chunk_size = *((int32_t*)metadata);

    read = fread(metadata, sizeof(char), 2, f);
    if (read != 2)
        return false;
    total_read += read;
    data->format = *((int16_t*)metadata);

    read = fread(metadata, sizeof(char), 2, f);
    if (read != 2)
        return false;
    total_read += read;
    data->channels = *((int16_t*)metadata);

    read = fread(metadata, sizeof(char), 4, f);
    if (read != 4)
        return false;
    total_read += read;
    data->sample_rate = *((int32_t*)metadata);

    read = fread(metadata, sizeof(char), 4, f);
    if (read != 4)
        return false;
    total_read += read;
    data->byte_rate = *((int32_t*)metadata);

    read = fread(metadata, sizeof(char), 2, f);
    if (read != 2)
        return false;
    total_read += read;
    // blockAlignment, dont care

    read = fread(metadata, sizeof(char), 2, f);
    if (read != 2)
        return false;
    total_read += read;
    data->bits_per_sample = *((int16_t*)metadata);

    int extra = chunk_size - total_read;
    if (extra)
    {   // skip past extra metadata
        read = fread(metadata, sizeof(char), extra, f);
        if (read != extra)
            return false;
    }

    return true;
}

bool read_wav_data(FILE* f, WavData* data, unsigned char** buffer)
{
    *buffer = NULL;
    data->size = 0;
    data->duration = 0;
    char metadata[5] = {'\0'};
    int read;

    // verify chunk id
    read = fread(metadata, sizeof(char), 4, f);
    if (read != 4)
        return false;
    if (strcmp(metadata, "data") != 0)
    {
        printf("data subchunk id invalid: %s\n", metadata);
        return false;
    }

    // get size of wav data
    read = fread(metadata, sizeof(char), 4, f);
    if (read != 4)
        return false;
    data->size = *((int32_t*)metadata);
    data->duration = ((float)data->size) / data->byte_rate;

    if (data->size == 0)
    {
        printf("PCM data size is 0.\n");
        return false;
    }

    // load pcm data to buffer
    *buffer = (unsigned char*)malloc(sizeof(unsigned char) * data->size);
    read = fread(*buffer, sizeof(char), data->size, f);
    if (read != data->size)
    {
        printf("Failed to read PCM data of size %d\n", data->size);
        return false;
    }

    return true;
}

/*
 *  Reference:
 *  http://enigma-dev.org/forums/index.php?topic=730.0
 */
 
// returns buffer id.  make sure to free *buffer after binding to an ALbuffer
int load_wav_file(char *fn, unsigned char** buffer)
{    
    *buffer = NULL;
    WavData* data = NULL;
    int id = get_free_wav_data(&data);
    if (data == NULL)
    {
        printf("No free wav data slots found.\n");
        return -1;
    }

    // open file
    char* fullpath = (char*)malloc(sizeof(char) * (strlen(base_path) + strlen(fn) + 1));
    sprintf(fullpath, "%s%s", base_path, fn);
    FILE* f = fopen(fullpath, "rb");
    free(fullpath);
    if (f == NULL)
    {
        printf("Error opening file: %s\n", fn);
        return -1;
    }

    // check header
    if (!is_valid_wav(f))
    {
        printf("%s is not a WAV file\n", fn);
        fclose(f);
        return -1;
    }

    // read metadata
    if (!read_wav_fmt_subchunk(f, data))
    {
        printf("%s has an invalid format chunk\n", fn);
        fclose(f);
        return -1;
    }

    // read wav data
    if (!read_wav_data(f, data, buffer))
    {
        printf("Failed to read data subchunk of %s\n", fn);
        return -1;
    }
    if (*buffer == NULL)
    {
        printf("WAV data read failure: buffer is NULL\n");
        return -1;
    }
    
    data->in_use = true;
    fclose(f);
    
    return id;
}

void release_wav_data(int buffer_id)
{
    if (buffer_id < 0 || buffer_id >= MAX_WAV_BUFFERS)
    {
        printf("WARNING: close_wav_file -- buffer_id %d invalid\n", buffer_id);
        return;
    }

    wav_buffers[buffer_id].in_use = false;
}

WavData* get_loaded_wav_data(int buffer_id)
{
    if (buffer_id < 0 || buffer_id >= MAX_WAV_BUFFERS)
    {
        printf("WARNING: get_loaded_buffer -- buffer_id %d invalid\n", buffer_id);
        return NULL;
    }
    return &wav_buffers[buffer_id];
}

void init_wav_buffers()
{
    static int inited = 0;
    if (inited++)
    {
        printf("WARNING: attempt to call init_wav_buffers more than once\n");
        return;
    }

    wav_buffers = (WavData*)calloc(MAX_WAV_BUFFERS, sizeof(WavData));
}

void teardown_wav_buffers()
{
    if (wav_buffers != NULL)
    {
        for (int i=0; i<MAX_WAV_BUFFERS; i++)
            if (!wav_buffers[i].in_use)
                release_wav_data(i);
        free(wav_buffers);
    }
    wav_buffers = NULL;
}


}
