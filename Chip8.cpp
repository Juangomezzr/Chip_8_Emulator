//
// Created by jugor on 16/05/2025.
//
#include <iostream>
#include "Chip8.h"
#include <cstring>
#include <random>
Chip8::Chip8(const char rom_name[],Chip8configuration configuration) {
    error = false;

    config = configuration;
    uint16_t entrypoint = 0x200;
    uint8_t chip8_fontset[80] =
            {
                    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                    0x20, 0x60, 0x20, 0x20, 0x70, // 1
                    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
            };


//registros
    PC = entrypoint;
    sp = -1;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;
//definiendo valor default
    memset(V,0, sizeof(V));

    memset(memoryRam,0,sizeof(memoryRam));

    memset(stack,0,sizeof(stack) );
    memset(display,0,sizeof(display));
    memset(keyboard,0,sizeof(keyboard));

    //Load Font
    for (int i = 0; i<80;i++){
        memoryRam[i] = chip8_fontset[i];
    }

    //open rom

    if(rom_name != nullptr) {
        FILE *rom = fopen(rom_name, "rb");
        if (rom) {
            //rom size
            fseek(rom, 0, SEEK_END);
            const size_t romSize = ftell(rom);
            const size_t max_size = sizeof(memoryRam) - entrypoint;
            rewind(rom);

            if (romSize < max_size) {
                if (fread(&(memoryRam[entrypoint]), romSize, 1, rom) != 1) {
                    printf("problemas al cargar rom\n");
                    error = true;
                } else {
                    printf("rom loaded succesfully\n");

                }
            } else {
                printf("rom to big\n");
                error = true;
            }

        } else {
            printf("rom failed to load\n");
            error = true;
        }

        fclose(rom);
    }else{
        SDL_Log("Rom Vacia");
    }

}

void Chip8::drawDisplay(SDL_Renderer *renderer,SDL_Texture *displayTexture){





    SDL_FRect rect = {0, 0,config.scale , config.scale};


    uint8_t b_r =config.B_colour[0];
    uint8_t b_g = config.B_colour[1];
    uint8_t b_b = config.B_colour[2];
    uint8_t b_a = config.B_colour[3];




    // initializing foreground color
    uint8_t f_r = config.F_colour[0];
    uint8_t f_g =config.F_colour[1];
    uint8_t f_b = config.F_colour[2];
    uint8_t f_a = config.F_colour[3];




    SDL_SetRenderTarget(renderer, displayTexture); // renderizar en la textura
    SDL_RenderClear(renderer);

    //iterating the display to draw each pixel
    for(int i = 0; i < 2048; i ++){
        rect.x = (i %64 ) * config.scale; // each 64 pixels in x-axis wrap
        rect.y = (i /64) * config.scale; // each 64 pixels is a new y level in tje y axis
        if(display[i]){
            SDL_SetRenderDrawColor(renderer,f_r,f_g,f_b,f_a);

        }else{
            SDL_SetRenderDrawColor(renderer, b_r, b_g, b_b, b_a);
        }
        SDL_RenderFillRect(renderer,&rect);

    }

    SDL_SetRenderTarget(renderer, NULL); // Volver al render normal


}
bool Chip8::fetch(){
    started = true;
    if (PC < 0xFFF) {

        pcAnterior = PC;//Save pc to show it lateer
        instruccionActual = (memoryRam[PC] << 8) | memoryRam[PC + 1];//actual inst

        PC += 2;//Act pc
        return true;
    } else {
        SDL_Log("End of of program no more memory");
        return false;
    }

}
void Chip8::formatInstruction () {

    uint16_t instruction = instruccionActual;
    inst_format formatedInstrution ;
    formatedInstrution.h3 = instruction >> 12 & 0xF;
    formatedInstrution.h2 = instruction >> 8 & 0xF;
    formatedInstrution.h1 = instruction >> 4 & 0xF;
    formatedInstrution.h0 = instruction >> 0 & 0xF;

    instruccionActual_Formated = formatedInstrution;

}

void Chip8::executeInst(){
    inst_format ins = instruccionActual_Formated;

    uint8_t h0 = ins.h0;
    uint8_t h1 = ins.h1;
    uint8_t h2 = ins.h2;
    uint8_t h3 = ins.h3;
    uint16_t NNN = h2 << 8| h1 << 4| h0;
    uint8_t NN = h1 << 4| h0;
    uint8_t x = h2 ;
    uint8_t y = h1 ;


    uint16_t resultadoRes;
    uint16_t resultadoSum;
    /*
     * h3 => hex3 opcode
     * NNN => 12 bits inmetdiate
     * NN=> 8 bits inmediate
     * N => 4 bits inmediate
     * X,Y => Vx,Vy registers
     *
     *
     */




    //Comprobar h3 ( opcode)
    switch (h3){
        case 0x0://DOne
            if (h2 == 0x0) {
                if(h0 == 0xE){

                    //Return from subrutine
                    if(sp > -1){

                    PC = stack[sp];
                    sp-=1;

                    }else{
                        SDL_Log("Line 191 chip8.cpp [Emptsie Stack] PC 0x%04x",pcAnterior);
                    }

                } else if ( h0 == 0)
                {
                    if(h1 != 0) {
                        //Clear Display
                        memset(display, 0, sizeof(display));
                        SDL_Log("Clear");
                    }else{

                        //nop
                    }
                }


            }else{
                   //nop
            }
            break;

        case 0x1:


            //IBM logo sems to use PC = NNN +2 research about it¿¿I dont think so¿¿
            PC = NNN ;
            break;
        case 0x2:
            if(sp < 16) {

                sp += 1;
                stack[sp] = PC;
                PC = NNN ;
            }else{
                SDL_Log("Line 218 : chip8-cpp [Stack OverFlow] PC 0x%04x ",pcAnterior);
            }
            break;
        case 0x3:
            if(V[x] == NN) PC += 2;
            break;
        case 0x4:
            if(V[x] != NN) PC +=2;
            break;
        case 0x5:
            if(V[x] == V[y]) PC+=2;
            break;
        case 0x6:

            //0x6XNN Vx == NN
            V[x] = NN;
            break;
        case 0x7:
            //0x7xnn Vx += nn
            V[x] += NN;
            break;
        case 0x8:
            switch (h0) {
                case 0:
                    V[x] = V[y];
                    break;
                case 1:
                    V[0xf] = 0;
                    V[x]= V[x]|V[y];
                    break;
                case 2:
                    V[0xf] = 0;
                    V[x]= V[x]&V[y];
                    break;
                case 3:
                    V[0xf] = 0;
                    V[x]=V[x]^V[y];
                    break;
                case 4:


                    resultadoSum = V[x] +V[y];
                    if(resultadoSum < V[x]){
                        V[0xf] = 1;
                    }else{
                        V[0xf]=0;
                    }
                    V[x]=resultadoSum;
                    break;
                case 5:

                    resultadoRes = V[x]-V[y];
                    if (V[x] >= V[y]){
                        V[0xf] = 1;
                    }else{
                        V[0xF] = 0;
                    }
                    V[x] = resultadoRes;

                    break;
                case 6:
                    if(!config.SHIFT) {
                        V[x]=V[y];
                    }
                        V[0xf] = V[x] & 0x0001;
                        V[x] = V[x] >> 1;

                    break;
                case 7:

                    resultadoRes = V[y]-V[x];
                    if (V[y] >= V[x]){
                        V[0xf] = 1;
                    }else{
                        V[0xF] = 0;
                    }
                    V[x] = resultadoRes;
                    break;
                case 0xE:
                    if(!config.SHIFT) {
                        V[x]=V[y];
                    }
                        V[0xf] = V[x] & 0x8000;
                        V[x] = V[x] << 1;

                    break;
                default:
                    SDL_Log("Intruction 0x%04x not implemented", instruccionActual);
                    break;
            }

            break;
        case  0x9:
            if(V[x]!=V[y]){
                PC+=2;
            }
            break;

        case    0xa:
            //ANNN I = NNN
            I = NNN;
            break;
        case 0xb:
            if(!config.BNNN_quirk) {//Aditional feature se interenes
                PC = NNN + V[0];
            }else{
                PC = V[x] +NNN;

            }
            break;
        case 0xc:
            V[x] = NN & rand() % 256;
            break;
        case 0xD:

            //Display
            // 0xDXYN read N bytes from memory, starting in I and display it in the position Vx, Vy
            uint8_t pos_x;
            uint8_t  pos_y ;
            pos_y = V[y]%32;
            pos_x = V[x]%64;
            V[0xF] =0;

            /*
             *
             * Sprite  1 0 0 0 1 1 1 0 1 0
             * Display 1 1 1 1 0 1 0 0 0 1
             * =       0 1 1 1 1 0 1 0 1 1
             *
             *
             * */

            for (int row = 0; row < h0; row+=1) {
                if (pos_y >= 32) continue;
                uint8_t sprite = memoryRam[I + row];

                for (int pixel = 0; pixel < 8; ++pixel) {
                    if (pos_x+pixel >= 64) continue; // Corta a la derecha
                    if(  display[(pos_y*64) + pos_x + pixel ]  == 1   && (sprite >> (7-pixel) & 1) == 1   )
                    {

                        display[(pos_y*64) + pos_x + pixel ] = 0;
                        V[0xf] = 1;

                    }else if(display[(pos_y*64) + pos_x + pixel ]  == 0   && (sprite >> (7-pixel) & 1) == 1)
                    {
                        display[(pos_y*64) + pos_x + pixel] = 1;




                    }else if(display[(pos_y*64) + pos_x + pixel ]  == 1   && (sprite >> (7-pixel) & 1) == 0){

                        display[(pos_y*64) + pos_x + pixel] = 1;

                    }else if(display[(pos_y*64) + pos_x + pixel ]  == 0   && (sprite >> (7-pixel) & 1) == 0){
                        display[(pos_y*64) + pos_x + pixel] = 0;




                    }
                }
                pos_y+=1;
            }
            draw_flag = true;
            break;
        case 0xe:

                if (NN == 0x9e) {
                    if(tecla_pulsada) {
                        if (presed_key == V[x]) {
                            PC += 2;
                        }
                    }

                } else if (NN == 0xa1) {
                    if (presed_key != V[x]) {
                        PC += 2;
                    }
                }

            break;
        case 0xF:
            switch (NN) {

                case 0x07:
                    V[x] = delay_timer;
                    break;
                case 0x15:
                    delay_timer = V[x];
                    break;
                case 0x18:
                    sound_timer = V[x];
                    break;
                case 0x1e:
                    I += V[x];
                    break;
                case 0x0a:
                ////NO tocar
                    //Get key
                    if(tecla_pulsada){

                        V[x] = presed_key;
                        state = 1;
                        PC-=2;

                    }else if(state == 1){
                        state = 0;
                    }else{
                        PC-=2;
                    }

                    break;
                case 0x29:
                    I = (V[x] * 5);
                    break;
                case 0x33:
                    uint8_t numAux;
                    numAux = V[x];
                    memoryRam[I+2] = numAux%10;
                    numAux/=10;
                    memoryRam[I+1] = numAux%10;
                    numAux/=10;
                    memoryRam[I] = numAux;
                    break;
                case 0x55:
                    for ( uint8_t i = 0; i <= x ; ++i) {

                        memoryRam[I+i] = V[i];

                    }
                    if(config.MEM_QUIRK) {
                        I += x;
                    }

                    break;
                case 0x65:
                    for (uint8_t i = 0; i <= x; ++i) {

                        V[i] = memoryRam[I+i];
                    }
                    if(config.MEM_QUIRK) {
                        I += x;
                    }
                    break;

                default:
                    SDL_Log("Instruction 0x%04x [Not Implemented] ",instruccionActual);

            }

            break;

        default:
            SDL_Log("Instruction at PC -> 0x%04x -- Cod 0x%04x : Not Implemented",pcAnterior,instruccionActual);
            break;
    }

}
