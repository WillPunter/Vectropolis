/*
	sound.h
	
	This includes all of the sound-related functions, including functions for initialising the sound engine, playing sounds and setting the volume
*/

//sound functions
//initialise sound engine
HRESULT initialiseSoundEngine(){
	//create result
	HRESULT result = S_OK;
	
	//create wave format for music
	WAVEFORMATEX musicWaveFormat = {0};
	musicWaveFormat.wFormatTag = WAVE_FORMAT_PCM; //use pulse code modulation format (this is where amplitude is sampled at a regular frequency)
	musicWaveFormat.nChannels = 2; //stereo
	musicWaveFormat.nSamplesPerSec = 44100; //standard sample rate for .wav file
	musicWaveFormat.nAvgBytesPerSec = musicWaveFormat.nSamplesPerSec * musicWaveFormat.nChannels * 2; //data stored per second = number of samples * number of channels * 2 bytes per sample
	musicWaveFormat.nBlockAlign = musicWaveFormat.nChannels * 2; //size of each two-channel sample
	musicWaveFormat.wBitsPerSample = 16; //number of bits in a 2-byte sample
	musicWaveFormat.cbSize = 0x6164; //size of typicaly wave format

	//initialise standard, multithreaded COM
	result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	
	//check if COM failed to initialise
	if(result != S_OK){
		return result;
	};
	
	//create xaudio2 intance
	result = XAudio2Create(&soundHandler.xaudio2, 0, XAUDIO2_ANY_PROCESSOR);
	
	//check if initialisation failed
	if(result != S_OK){
		return result;
	};
	
	//create mastering voice from xaudio2 interface
	//instance pointer, pointer to where mastering voice will be stored, number of channels, sample rate, flags, device id, effect chain, stream category
	result = soundHandler.xaudio2->lpVtbl->CreateMasteringVoice(soundHandler.xaudio2, &soundHandler.masteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL, 0);
	
	//create wave format for sound effects
	WAVEFORMATEX sfxWaveFormat = {0};
	sfxWaveFormat.wFormatTag = WAVE_FORMAT_PCM; //use pulse code modulation format (this is where amplitude is sampled at a regular frequency)
	sfxWaveFormat.nChannels = 1; //stereo
	sfxWaveFormat.nSamplesPerSec = 44100; //standard sample rate for .wav file
	sfxWaveFormat.nAvgBytesPerSec = sfxWaveFormat.nSamplesPerSec * sfxWaveFormat.nChannels * 2; //data stored per second = number of samples * number of channels * 2 bytes per sample
	sfxWaveFormat.nBlockAlign = sfxWaveFormat.nChannels * 2; //size of each two-channel sample
	sfxWaveFormat.wBitsPerSample = 16; //number of bits in a 2-byte sample
	sfxWaveFormat.cbSize = 0x6164; //size of typicaly wave format

	//check if initialisation failed
	if(result != S_OK){
		return result;
	};
	
	//attempt to creaet music src voice
	result = soundHandler.xaudio2->lpVtbl->CreateSourceVoice(soundHandler.xaudio2, &soundHandler.musicSrcVoice, &musicWaveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);
	
	//set music volume
	soundHandler.musicSrcVoice->lpVtbl->SetVolume(soundHandler.musicSrcVoice, gameSave.musicVolume, XAUDIO2_COMMIT_NOW);
	
	//initialise source voices
	int i;
	
	for(i = 0; i < NUMBER_OF_SFX_SRC_VOICES; i++){
		//create sound effect source voice
		//xaudio2 instance pointer, sfx pointer to source voice interface, &sfxWaveFormat, flags, frequency ratio, voice callback, send list, effect chain
		result = soundHandler.xaudio2->lpVtbl->CreateSourceVoice(soundHandler.xaudio2, &soundHandler.sfxSrcVoices[i], &sfxWaveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);
	
		//check if src voice failed to create
		if(result != S_OK){
			return result;
		};
		
		//set volume of source voice
		soundHandler.sfxSrcVoices[i]->lpVtbl->SetVolume(soundHandler.sfxSrcVoices[i], gameSave.soundVolume, XAUDIO2_COMMIT_NOW);
	};
	
	return result;
};

//play sound
void playSound(Sound * sound){
	//check if sound is turned on
	if(soundOn){
		//stream buffer to source voice
		soundHandler.sfxSrcVoices[soundHandler.srcVoiceSelecter]->lpVtbl->SubmitSourceBuffer(soundHandler.sfxSrcVoices[soundHandler.srcVoiceSelecter], &sound->buffer, NULL);
	
		//play sound
		soundHandler.sfxSrcVoices[soundHandler.srcVoiceSelecter]->lpVtbl->Start(soundHandler.sfxSrcVoices[soundHandler.srcVoiceSelecter], 0, XAUDIO2_COMMIT_NOW);
	
		//increment source voice
		soundHandler.srcVoiceSelecter = (soundHandler.srcVoiceSelecter + 1) % NUMBER_OF_SFX_SRC_VOICES;
	};
};

//stop sound
void stopSound(){
	//iterate through source voices
	int i;
	
	for(i = 0; i < NUMBER_OF_SFX_SRC_VOICES; i++){
		soundHandler.sfxSrcVoices[i]->lpVtbl->Stop(soundHandler.sfxSrcVoices[i], 0, 0);
	};
};

//set sound volume
void setSoundVolume(){
	//iterate through source voices
	int i;
	
	for(i = 0; i < NUMBER_OF_SFX_SRC_VOICES; i++){
		soundHandler.sfxSrcVoices[i]->lpVtbl->SetVolume(soundHandler.sfxSrcVoices[i], gameSave.soundVolume, XAUDIO2_COMMIT_NOW);
	};
};

//sound volume up
void soundVolumeUp(){
	gameSave.soundVolume += 0.1;
	
	if(gameSave.soundVolume > 1){
		gameSave.soundVolume = 0;
	};
	
	setSoundVolume();
};

//sound volume down
void soundVolumeDown(){
	gameSave.soundVolume -= 0.1;
	
	if(gameSave.soundVolume < 0){
		gameSave.soundVolume = 1;
	};
	
	setSoundVolume();
};

//play music
void playMusic(Sound * sound){
	//check if track has changed
	if(soundHandler.currentMusic != sound){
		//stop all music
		stopMusic();
		
		//flush source buffer to get rid of audio data
		soundHandler.musicSrcVoice->lpVtbl->FlushSourceBuffers(soundHandler.musicSrcVoice);
		
		//set track
		soundHandler.currentMusic = sound;
	};
	
	//check if music is turned on
	if(musicOn){
		//stream buffer to source voice
		soundHandler.musicSrcVoice->lpVtbl->SubmitSourceBuffer(soundHandler.musicSrcVoice, &sound->buffer, NULL);
	
		//play sound
		soundHandler.musicSrcVoice->lpVtbl->Start(soundHandler.musicSrcVoice, 0, XAUDIO2_COMMIT_NOW);
	};
};

//stop music
void stopMusic(){
	//stop music immediately
	soundHandler.musicSrcVoice->lpVtbl->Stop(soundHandler.musicSrcVoice, 0, 0);
};

//set music volume
void setMusicVolume(){
	//set music volume
	soundHandler.musicSrcVoice->lpVtbl->SetVolume(soundHandler.musicSrcVoice, gameSave.musicVolume, XAUDIO2_COMMIT_NOW);
};

//music volume up
void musicVolumeUp(){
	gameSave.musicVolume += 0.1;
	
	if(gameSave.musicVolume > 1){
		gameSave.musicVolume = 0;
	};
	
	setMusicVolume();
};

//music volume down
void musicVolumeDown(){
	gameSave.musicVolume -= 0.1;
	
	if(gameSave.musicVolume < 0){
		gameSave.musicVolume = 1;
	};
	
	setMusicVolume();
};

//miscellaneous settings functions
void xSensitivityUp(){
	gameSave.xSensitivity += 0.1;
	
	if(gameSave.xSensitivity > 1){
		gameSave.xSensitivity = 0.1;
	};
};

void xSensitivityDown(){
	gameSave.xSensitivity -= 0.1;
	
	if(gameSave.xSensitivity < 0){
		gameSave.xSensitivity = 1;
	};
};

void ySensitivityUp(){
	gameSave.ySensitivity += 0.1;
	
	if(gameSave.ySensitivity > 1){
		gameSave.ySensitivity = 1;
	};
};

void ySensitivityDown(){
	gameSave.ySensitivity -= 0.1;
	
	if(gameSave.ySensitivity < 0){
		gameSave.ySensitivity = 1;
	};
};

//save functions
//write save
void writeSave(Save * save){
	//retrieve path to my games folder
	char path[128];
	sprintf(path, "%s/Saved Games/%s/save.dat", getenv("USERPROFILE"), GAME_TITLE);

	//open file
	FILE * file = fopen(path, "wb");
	printf("\n\n\n\nSaving data....");
	
	//display leaderboard
	int i;
	printf("Read Save\n");
	for(i = 0; i < NUMBER_OF_HIGH_SCORES; i++){
		printf("%s - %d\n", gameSave.highScoreNames[i], gameSave.highScoreScores[i]);
	};
		
	//check if file opened
	if(file){
		//write save to file
		fwrite(save, sizeof(Save), 1, file);
		
		//close file
		fclose(file);
	};
};

//load save
int loadSave(Save * save){
	//retrieve path to my games folder
	char path[128];
	sprintf(path, "%s/Saved Games/%s/save.dat", getenv("USERPROFILE"), GAME_TITLE);

	//open file
	FILE * file = fopen(path, "rb");
	
	//check if file opened
	if(file){
		//write save to file
		fread(save, sizeof(Save), 1, file);

		//close file
		fclose(file);
		
		//display leaderboard
		int i;
		printf("Read Save\n");
		for(i = 0; i < NUMBER_OF_HIGH_SCORES; i++){
			printf("%s - %d\n", gameSave.highScoreNames[i], gameSave.highScoreScores[i]);
		};

		//return successful
		return 1;
	};

	//return unsuccessful
	return 0;
};

//add score 
void addScore(Save * save, char name[STR_LENGTH], int score){
	//sort save scores (ascending order)
	sortSaveScores(save, 1);
	
	//create loop counter
	int found = -1;
	int i;
	
	for(i = NUMBER_OF_HIGH_SCORES - 1; i >= 0; i --){
		if(save->highScoreScores[i] < score){
			//leaderboard index found
			found = i;
			break;
		};
	};
	
	//check if found
	if(found > -1){
		//shift high scores backwards
		for(i = 1; i <= found; i++){
			//shift backwards
			save->highScoreScores[i - 1] = save->highScoreScores[i];
			strcpy(save->highScoreNames[i - 1], save->highScoreNames[i]);
		};
		
		//add score into correct place
		save->highScoreScores[found] = score;
		strcpy(save->highScoreNames[found], name);
		
		//save file
		writeSave(save);
	};
};

//sort save scores
void sortSaveScores(Save * save, int ascending){
	//iterate through save
	int i;
	int j;
	
	for(i = 1; i < NUMBER_OF_HIGH_SCORES; i++){
		j = i;
		
		while(j >= 1 && ((save->highScoreScores[j] < save->highScoreScores[j - 1] && ascending) || (save->highScoreScores[j] > save->highScoreScores[j - 1] && !ascending))){
			//swap scores
			int temp = save->highScoreScores[j];
			save->highScoreScores[j] = save->highScoreScores[j - 1];
			save->highScoreScores[j - 1] = temp;
			
			//swap names
			char name[STR_LENGTH];
			strcpy(name, save->highScoreNames[j]);
			strcpy(save->highScoreNames[j], save->highScoreNames[j - 1]);
			strcpy(save->highScoreNames[j - 1], name);
			j -= 1;
		};
	};
};