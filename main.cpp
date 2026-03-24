
#include <iostream>
#include <cstdio>
#include <chrono>


#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"
#include <SDL3/SDL.h>
#include "Chip8.h"



typedef struct configuration {
    int width = 1080;
    int height = 820;
    const char *romSelected = nullptr;

    bool debug_mode = true;
    int velocity = 1;//Numero de instrucciones por ciclo de while, el debug deja de mostrar las cosas correctamente



}AppConfig;



//Sdl
SDL_Window * Chip_8Window;
SDL_Renderer * Chip_8Renderer;


// Config
const int sampleRate = 48000 ;
const float beepFrequency = 440.0f;
const int bufferSamples = 1024 ;
static SDL_AudioStream* audioStream = nullptr;
static std::vector<int16_t> beepBuffer;


void init_audio() {
    //SDL audio things
    SDL_AudioSpec spec{};
    spec.freq = sampleRate;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;

    audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!audioStream) {
        SDL_Log("AudioStream error: %s", SDL_GetError());
        return;
    }

    SDL_ResumeAudioStreamDevice(audioStream);
    //---------------------------------------


    // Generar onda senoidal para beep
    beepBuffer.resize(bufferSamples);
    float phase = 1.0f;
    float increment = 2.0f * M_PI * beepFrequency / sampleRate;

    for (int i = 0; i < bufferSamples; ++i) {
        float t = static_cast<float>(i) / bufferSamples; // Progreso entre 0.0 y 1.0
        float amplitude = sinf(M_PI * t); // Fade in/out suave (seno de media onda)
        float sample = sinf(phase) * amplitude * 3000;
        beepBuffer[i] = static_cast<int16_t>(sample);
        phase += increment;
    }
}

// Función para reproducir el beep si sound_timer > 0
void play_chip8_beep() {
    if ( audioStream) {
        SDL_PutAudioStreamData(audioStream, beepBuffer.data(), beepBuffer.size() * sizeof(int16_t));
        SDL_FlushAudioStream(audioStream);
    }
}


//Configuration
AppConfig configuration;

Chip8configuration configuration8;


int main() {
//----------------Inicializar_Chip---------------------

    Chip8 _chip8 = Chip8(configuration.romSelected,configuration8);

    //Stop program if error initializing CHIP_8
    if (_chip8.error) {
        SDL_Log("Error initializing chip_8");
        return -1;
    }

//-------------------------------------------------------
    //Seting Up SDL, Window and renderer
    if (!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)) {
        SDL_Log("SDL failed to initialize");
        return -2;
    }

    SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE;

    Chip_8Window = SDL_CreateWindow("Chip-8", configuration.width, configuration.height, windowFlags);

    if (Chip_8Window == NULL) {
        printf("Fail creating window");
        return -1;
    }

    Chip_8Renderer = SDL_CreateRenderer(Chip_8Window, NULL);

    if (Chip_8Renderer == NULL) {
        printf("Fail creating renderer");
        return -1;
    }


    SDL_SetRenderDrawColor(Chip_8Renderer,0x00,0x50,0x5f,0xff);
    SDL_RenderClear(Chip_8Renderer);

    SDL_Texture* chip8_texture = SDL_CreateTexture(
            Chip_8Renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            64 * _chip8.config.scale,
            32 * _chip8.config.scale
    );

//--------SOUND--------------------------------


init_audio();
//----------------------------------------------
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;

    //Flags for Input/output management
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    // Setup Dear ImGui style
    ImGui::StyleColorsClassic();



    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(Chip_8Window, Chip_8Renderer);
    ImGui_ImplSDLRenderer3_Init(Chip_8Renderer);

//---------------------------------------------

//Outside cicle variables----------------------

    bool running = true;
    bool stop = true;
    bool next = false;
    auto lastExecutionTime = std::chrono::high_resolution_clock::now();

    bool colorpicker = true;


//------------Style-------
    uint32_t colorTable_izq = IM_COL32(255, 90, 10, 200);
//----------------------------------------------

    while (running) {

        //Handle stoping
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            // If they are events in the poll enter the switch and solve it

            ImGui_ImplSDL3_ProcessEvent(&event);
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED :
                    if (event.window.windowID == SDL_GetWindowID(Chip_8Window)) {
                        running = false;
                    }
                    break;
                case SDL_EVENT_KEY_DOWN:
                    _chip8.tecla_pulsada = true;
                    _chip8.released = false;

                    break;
                default:
                    _chip8.tecla_pulsada = false;
                break;

            }
            if(_chip8.tecla_pulsada){
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        case SDLK_1:

                            _chip8.presed_key = 1;
                            break;
                        case SDLK_2:

                            _chip8.presed_key = 2;
                            break;
                        case SDLK_3:

                            _chip8.presed_key = 3;
                            break;
                        case SDLK_4:

                            _chip8.presed_key = 0xc;
                            break;
                        case SDLK_Q:

                            _chip8.presed_key = 4;
                            break;
                        case SDLK_W:

                            _chip8.presed_key = 5;
                            break;
                        case SDLK_E:

                            _chip8.presed_key = 6;
                            break;
                        case SDLK_R:
                            _chip8.presed_key = 0xd;
                            break;
                        case SDLK_A:

                            _chip8.presed_key = 0x7;
                            break;
                        case SDLK_S:

                            _chip8.presed_key = 0x8;
                            break;
                        case SDLK_D:

                            _chip8.presed_key = 0x9;
                            break;
                        case SDLK_F:

                            _chip8.presed_key = 0xe;
                            break;
                        case SDLK_Z:

                            _chip8.presed_key = 0xa;
                            break;
                        case SDLK_X:

                            _chip8.presed_key = 0x0;
                            break;
                        case SDLK_C:

                            _chip8.presed_key = 0xb;
                            break;
                        case SDLK_V:
                            _chip8.presed_key = 0xF;
                            break;

                    }

            }else{
                _chip8.presed_key = 0xff;
            }



        }

        //--------Emulator_Logics-----------------------------------------------------





            if(!stop | next) {
                if (next) next = false;
                for (int i = 0; i < configuration.velocity; ++i) {


                    auto now = std::chrono::high_resolution_clock::now();
                    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastExecutionTime);
                    if (delta.count() >= 1000 / 60) { // 60Hz = 16.666 ms por frame
                        if (_chip8.delay_timer > 0) {
                            _chip8.delay_timer--;  // Decrementa delay y sound timer
                        };
                        if (_chip8.sound_timer > 0) {


                            play_chip8_beep();
                            _chip8.sound_timer--;
                        }
                        lastExecutionTime = now;
                    }
                    //Fetch
                    //Leer una instruccion de memoria

                    if (_chip8.fetch()) {
                        //Decode
                        _chip8.formatInstruction();
                        //Emulate Chip8 inst
                        _chip8.executeInst();
                        //Draws the display into a texture
                    }

                    if(_chip8.draw_flag) {
                    _chip8.drawDisplay(Chip_8Renderer, chip8_texture);
                    _chip8.draw_flag = false;
                    }
                }
            }



        //--------------------------------------------------------------------------


        /*
         * La logica del emulador succede antes que la ui asi los cambios en el display succeden antes que la ui y la ui se
         * puede mostrar encima, si no dibujariamos la ui y
         * despues el diaplay por encima lo que sobrescribiria la ui,
         * */
        //Buscar forma de separar UI y Display ¡¡¡¡¡Echo¡¡¡¡¡¡


// Start the Dear ImGui frame, se pinta la ui por encima del display




//-----------GUI----------------------------------------
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        //----------GUI_Render---------
        {

            ///---------------DOCKSpace_Setup--------------------------

            static ImGuiDockNodeFlags dockspace_flags =ImGuiDockNodeFlags_NoUndocking
                    |ImGuiDockNodeFlags_PassthruCentralNode;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags main_window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking|ImGuiWindowFlags_AlwaysAutoResize;


            //Docked window param
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);  // Same as below
            ImGui::SetNextWindowSize(
                    viewport->WorkSize); // Makes the dockspace window same size as the main window worsize
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));//Para eliminar el padding

            main_window_flags |= ImGuiWindowFlags_NoTitleBar // Quitar nombre, header, barra superior de la ventana
                            | ImGuiWindowFlags_NoCollapse //
                            |ImGuiWindowFlags_NoResize //Como sera siempre del tamaño del viewport no se podra cambiar el tamaño
                            | ImGuiWindowFlags_NoMove; // pa que no se mueva

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.


            ///-----------------Initializing_DockSpace------------
            ImGui::Begin("DockSpace", nullptr, main_window_flags);
            ImGui::PopStyleVar();//para restaurar los stilos anteriores

            // Submit the DockSpace
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");// Id unico no hace falta en este proyecto pero lo dejo
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
                             dockspace_flags);
            // Se puede hacer de otra forma mas simple con
            // DockSpaceViewport pero usaremos esta al ser mas consistente
            // size(0.0,0.0) its going to be resize to the viewport worksize

            ///----------------DockSpace_Content_________________


                if (ImGui::BeginMenuBar()) {
                    if(ImGui::BeginMenu("Files")){
                        IGFD::FileDialogConfig config;config.path = "../roms";
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".ch8", config);
                        ImGui::EndMenu();
                    }
                    if(ImGui::BeginMenu("Quirks")){

                        if(configuration8.SHIFT){
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255)); // verde
                        }else{
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255)); // Rojo
                        }

                        if(ImGui::Button("SHIFT [ 0x8XY6 and 0x8XY7 ]")){
                            configuration8.SHIFT = !configuration8.SHIFT;
                            _chip8 = Chip8(configuration.romSelected,configuration8);
                        }
                        ImGui::PopStyleColor();

                        if(configuration8.BNNN_quirk){
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255)); // verde
                        }else{
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255)); // Rojo
                        }
                        if(ImGui::Button("JUMP [ BNNN ] ")){
                            configuration8.BNNN_quirk = !configuration8.BNNN_quirk;
                            _chip8 = Chip8(configuration.romSelected,configuration8);
                        }
                        ImGui::PopStyleColor();



                        if(configuration8.MEM_QUIRK){
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255)); // verde
                        }else{
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255)); // Rojo
                        }
                        if(ImGui::Button("MEM QUIRK [ Modify I ] ")){
                            configuration8.MEM_QUIRK = !configuration8.MEM_QUIRK;
                            _chip8 = Chip8(configuration.romSelected,configuration8);
                        }
                        ImGui::PopStyleColor();
                        ImGui::EndMenu();
                    }


                    ImGui::EndMenuBar();
                    // display files
                    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) { // => will show a dialog
                        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                            configuration.romSelected = filePathName.c_str();
                            _chip8 = Chip8(configuration.romSelected,configuration8);
                            // action
                        }

                        // close
                        ImGuiFileDialog::Instance()->Close();
                    }
                }


                ///------------------PopUp--------------------------




                ///-------------------Rigth window--------------
                ImGuiWindowFlags secondaryWindowsflags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.2f, 1.0f));
                ImGui::Begin("Registro", nullptr, secondaryWindowsflags);



                ImGui::BeginGroup();
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(2, 2));
                ImGui::SetNextItemWidth(130);

                    if (ImGui::BeginTable( "registros", 2,  ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|
                        ImGuiTableFlags_SizingFixedFit|ImGuiTableFlags_NoHostExtendX|ImGuiTableFlags_Resizable)) {


                        ImGui::TableSetupColumn("Register");
                        ImGui::TableSetupColumn("Value");
                        ImGui::TableHeadersRow();

                        for (int i = 0; i < 16; ++i) {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("v%x",i);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("0x%04x",_chip8.V[i]);

                        }
                        ImGui::EndTable();

                    }
                    ImGui::PopStyleVar();

            ImGui::Text("Estado:");
            ImGui::Text("PC: 0x%x",_chip8.pcAnterior);
            ImGui::Text("Instruccion: 0x%04x",_chip8.instruccionActual);
            ImGui::Text("I: 0x%08x",_chip8.I);
            ImGui::Text("Delay Timer: %d",_chip8.delay_timer);

            ImGui::Text("Sound Timer: %d",_chip8.sound_timer);


            ImGui::EndGroup();


                    ///----Stack_Table----
                ImGui::SameLine();

                    if(ImGui::BeginTable("Memory_Map",2,ImGuiTableFlags_NoHostExtendX|ImGuiTableFlags_SizingFixedFit|ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg)){
                        ImGui::TableSetupColumn("Address");
                        ImGui::TableSetupColumn("Value");
                        ImGui::TableHeadersRow();
                        for (int i = _chip8.I ; i < _chip8.I +16; i++){
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%x",i);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("0x%04x",_chip8.memoryRam[i]);

                        }

                        ImGui::EndTable();
                    }


                ImGui::SameLine();

                ImGui::BeginGroup();
                if(ImGui::BeginTable("Stack",2,ImGuiTableFlags_Borders|ImGuiTableFlags_RowBg|
                                               ImGuiTableFlags_SizingFixedFit|ImGuiTableFlags_NoHostExtendX)){
                    ImGui::TableSetupColumn("Index");
                    ImGui::TableSetupColumn("Values");
                    ImGui::TableHeadersRow();
                    for (int i = 15; i >=0; i--){
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%x",i);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("0x%04x",_chip8.stack[i]);

                    }
                    ImGui::EndTable();
                }

                    ImGui::Text("SP: %x",_chip8.sp);
                ImGui::EndGroup();
                ImGui::SameLine();

// Grupo 3: elementos a la derecha
ImGui::BeginGroup();
    ImGui::BeginGroup();

            if(stop) {
                if(ImGui::Button("Run",ImVec2(50,30))){
                    stop = false;
                }
            } else if(ImGui::Button("Stop",ImVec2(50,30))){
                stop = true;
            }
            ImGui::SameLine();
            if(ImGui::Button("->",ImVec2(60,30)) && stop){
                next = true;
            }
            ImGui::SameLine();
            if(ImGui::Button("Reset",ImVec2(50,30))&& stop){

                    _chip8 = Chip8(configuration.romSelected,configuration8);


            }
    ImGui::EndGroup();
            ImGui::Dummy(ImVec2(0,0));
    ImGui::BeginGroup();

                //--------Teclado______

            ImGui::PushStyleColor(ImGuiCol_Button,0x80808080);
            ImVec2 sizeButton = ImVec2(50,50);

            ImGui::BeginGroup();
                if(ImGui::Button("1 = 1",sizeButton)){
                    _chip8.keyboard[0X0] = 1;
                }else{
                    _chip8.keyboard[0x0] = 0;
                };
                if(ImGui::Button("Q = 4",sizeButton)){
                    _chip8.keyboard[0X1] = 1;
                }else{
                    _chip8.keyboard[0x4] = 0;
                };
                if(ImGui::Button("A = 7",sizeButton)){
                    _chip8.keyboard[0X8] = 1;
                }else{
                    _chip8.keyboard[0x8] = 0;
                };
                if(ImGui::Button("Z = A",sizeButton)){
                    _chip8.keyboard[0XC] = 1;
                }else{
                    _chip8.keyboard[0xC] = 0;
                };
            ImGui::EndGroup();

            ImGui::SameLine(0,3);

            ImGui::BeginGroup();
            if(ImGui::Button("2 = 2",sizeButton)){
                _chip8.keyboard[0X1] = true;
            }else{
                _chip8.keyboard[0x1] = false;
            };if(ImGui::Button("W = 4",sizeButton)){
                _chip8.keyboard[0X5] = true;
            }else{
                _chip8.keyboard[0x5] = false;
            };if(ImGui::Button("S = 8",sizeButton)){
                _chip8.keyboard[0X9] = 1;
            }else{
                _chip8.keyboard[0x9] = 0;
            };if(ImGui::Button("X = 0",sizeButton)){
                _chip8.keyboard[0Xd] = 1;
            }else{
                _chip8.keyboard[0xd] = 0;
            };
            ImGui::EndGroup();

            ImGui::SameLine(0,3);
            ImGui::BeginGroup();
            if(ImGui::Button("3 = 3",sizeButton)){
                _chip8.keyboard[0X2] = 1;
            }else{
                _chip8.keyboard[0x2] = 0;
            };
            if(ImGui::Button("E = 6",sizeButton)){
                _chip8.keyboard[0X6] = 1;
            }else{
                _chip8.keyboard[0x6] = 0;
            };
            if(ImGui::Button("D = 9",sizeButton)){
                _chip8.keyboard[0Xa] = 1;
            }else{
                _chip8.keyboard[0xa] = 0;
            };
            if(ImGui::Button("C = B",sizeButton)){
                _chip8.keyboard[0Xe] = 1;
            }else{
                _chip8.keyboard[0xe] = 0;
            };
            ImGui::EndGroup();

            ImGui::SameLine(0,3);
            ImGui::BeginGroup();

            if(ImGui::Button("4 = C",sizeButton)){
                _chip8.keyboard[0X3] = 1;
            }else{
                _chip8.keyboard[0x3] = 0;
            };
            if(ImGui::Button("R = D",sizeButton)){
                _chip8.keyboard[0X7] = 1;
            }else{
                _chip8.keyboard[0x7] = 0;
            };
            if(ImGui::Button("F = E",sizeButton)){
                _chip8.keyboard[0Xb] = 1;
            }else{
                _chip8.keyboard[0xb] = 0;
            };
            if(ImGui::Button("F = 8",sizeButton)){
                _chip8.keyboard[0Xf] = 1;
            }else{
                _chip8.keyboard[0xf] = 0;
            };
            ImGui::EndGroup();

            ImGui::BeginGroup();
            ImGui::Text("Tecla Pulsada [ %d ]",_chip8.tecla_pulsada);


            ImGui::Text("Input [ %x ]",_chip8.presed_key);


            ImGui::Text("released? [ %d ]",_chip8.released);
            ImGui::EndGroup();
            ImGui::PopStyleColor();


    ImGui::EndGroup();

ImGui::EndGroup();


ImGui::End();
if(configuration.debug_mode) {
    ///-------------------Left------------------

    ImGui::Begin("Asm", nullptr, secondaryWindowsflags);


    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 1.f, 0.5f, 1.0f));   // Fondo del header
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));      // Fila impar
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    if (ImGui::BeginTable("registros2", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuterV
                                           | ImGuiTableFlags_BordersH | ImGuiTableFlags_RowBg |
                                           ImGuiTableFlags_ScrollY)) {


        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Inst");
        ImGui::TableHeadersRow();


        for (int i = 0x200; i < 4096; i += 2) {
            ImGui::TableNextRow();

            if (i == _chip8.PC) { ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, colorTable_izq); }  // Color rosado

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("0x%x", i);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("0x%04x", _chip8.memoryRam[i] << 8 | _chip8.memoryRam[i + 1]);
            ImGui::TableSetColumnIndex(2);


            if (!_chip8.started) {
                ImGui::PushID(i);

                char buffer[8]; // suficiente para "FFFF\0"
                snprintf(buffer, sizeof(buffer), "%04X", _chip8.memoryRam[i] << 8 | _chip8.memoryRam[i + 1]);

                if (ImGui::InputText("##i", buffer, sizeof buffer, ImGuiInputTextFlags_CharsHexadecimal)) {
                    // Intentar convertir el valor ingresado a uint16_t
                    unsigned int value = 0;
                    if (sscanf(buffer, "%04x", &value) == 1 && value <= 0xFFFF) {
                        _chip8.memoryRam[i] = static_cast<uint8_t>(value >> 8 & 0xff);
                        _chip8.memoryRam[i + 1] = static_cast<uint8_t>(value & 0xff);
                    }
                }
                ImGui::PopID();
            }
        }
        ImGui::EndTable();

    }
    ImGui::PopStyleColor(3);
    ImGui::End();

}

            ImGui::PopStyleColor(1);
                    ///----------Display-----------------
                    ImGuiWindowFlags displayflags =
                            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove ;

                    // ImGui::SetNextWindowSize(ImVec2(64 * _chip8.config.scale,32 * _chip8.config.scale));
                    //ImGui::SetNextWindowSizeConstraints(ImVec2(64* _chip8.config.scale,32 * _chip8.config.scale),
                    //ImVec2(64* _chip8.config.scale,32 * _chip8.config.scale));

                    if (ImGui::Begin("CHIP-8_Display", nullptr, displayflags)) {


                        //Scale deberia venir dada por la aplicacion no por el chip-8
                        ImVec2 display_size = ImVec2(64 * _chip8.config.scale, 32 * _chip8.config.scale);
                        ImGui::Image((ImTextureID) (void *) chip8_texture, display_size);
                    }
                    ImGui::End();
            ImGui::End(); // Of the dockSpace begin
        }

        //-------------------------------





        ImGui::Render();

        SDL_SetRenderDrawColor(Chip_8Renderer,0,0,0,0);
        SDL_RenderClear(Chip_8Renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), Chip_8Renderer);
        SDL_RenderPresent(Chip_8Renderer);

    }


        // Cleanup
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();


        SDL_DestroyAudioStream(audioStream);
        SDL_DestroyTexture(chip8_texture);
        SDL_DestroyRenderer(Chip_8Renderer);
        SDL_DestroyWindow(Chip_8Window);
        SDL_Quit();
        return 0;

}
