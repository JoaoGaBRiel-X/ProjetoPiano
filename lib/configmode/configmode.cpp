#include <configmode.h>


unsigned long timeProg1 = 0;
unsigned long timeProg2 = 0;
uint8_t flagvolume = 0;
uint8_t flagProg01 = 0;
uint8_t flagProg02 = 0;
uint8_t flagProg03 = 0;
uint8_t volume = 127;
uint8_t prg = 0;

void inicio_prg(){
            para_midi(78);
            para_midi(80);
            para_midi(82);
            toca_midi(81);
            delay(300);
            para_midi(81);
            toca_midi(81);
            delay(300);
            para_midi(81);
            toca_midi(82);
            delay(300);
            para_midi(82);
            prg = 1;
}

void handleConfigMode()
{
        if(flagProg02 && flagProg03)
    {

        if(!timeProg1){
            timeProg1 = millis();
        }
        if(millis() - timeProg1 >= 3000)
        {
            talkMIDI(0xc0, 118, 127);
            inicio_prg();
            uint8_t countTeclas = 0;
            uint8_t cmd = 0;
            while(prg)
            {
                char tecla = teclado_personalizado.getKey();
                if(tecla != 0)
                {
                    if((countTeclas == 0) && (  tecla == 0x7a ||    // tecla 0
                                                tecla == 0x78))     // tecla 1
                    {
                        tocaSucesso(countTeclas);
                        if(tecla == 0x78)
                        {
                            cmd += 100;
                        }
                        countTeclas++;
                        tecla = 0;
                    }
                    else if (countTeclas == 0)
                    {  
                        tocaErro();
                        tecla = 0;
                    }
                    if((countTeclas == 1) && (tecla != 0))   
                    {
                        if(cmd == 100)
                        {
                            if( tecla == 0x7a ||  // tecla 0
                                tecla == 0x78 ||  // tecla 1
                                tecla == 0x63) // tecla 2
                            {
                                tocaSucesso(countTeclas);
                                if(tecla == 0x78)
                                {
                                    cmd += 10;
                                }
                                if(tecla == 0x63)
                                {
                                    cmd += 20;
                                }
                                countTeclas++;
                                tecla = 0;
                            }
                            else
                            {  
                                tocaErro();
                                tecla = 0;
                            }
                        }
                        else
                        {
                            if(tecla == 0x7a) // tecla 0
                            {
                                tocaSucesso(countTeclas);
                            }
                            else if(tecla == 0x78) // tecla 1
                            {
                                tocaSucesso(countTeclas);
                                cmd += 10;
                            }
                            else if(tecla == 0x63) // tecla 2
                            {
                                tocaSucesso(countTeclas);
                                cmd += 20;
                            }
                            else if(tecla == 0x76) // tecla 3
                            {
                                tocaSucesso(countTeclas);
                                cmd += 30;
                            }
                            else if(tecla == 0x62) // tecla 4
                            {
                                tocaSucesso(countTeclas);
                                cmd += 40;
                            }
                            else if(tecla == 0x6e) // tecla 5
                            {
                                tocaSucesso(countTeclas);
                                cmd += 50;
                            }
                            else if(tecla == 0x6d) // tecla 6
                            {
                                tocaSucesso(countTeclas);
                                cmd += 60;
                            }
                            else if(tecla == 0x61) // tecla 7
                            {
                                tocaSucesso(countTeclas);
                                cmd += 70;
                            }
                            else if(tecla == 0x73) // tecla 8
                            {
                                tocaSucesso(countTeclas);
                                cmd += 80;
                            }
                            else if(tecla == 0x64) // tecla 9
                            {
                                tocaSucesso(countTeclas);
                                cmd += 90;
                            }
                            else
                            {
                                tocaErro();
                                continue;
                            }
                            countTeclas++;
                            tecla = 0;
                        }
                       
                    }
                    if(countTeclas == 2 && tecla != 0)
                    {
                        if(tecla == 0x78) // tecla 1
                        {
                            tocaSucesso(countTeclas);
                            cmd += 1;
                        }
                        if(tecla == 0x63) // tecla 2
                        {
                            tocaSucesso(countTeclas);
                            cmd += 2;
                        }
                        if(tecla == 0x76) // tecla 3
                        {
                            tocaSucesso(countTeclas);
                            cmd += 3;
                        }
                        if(tecla == 0x62) // tecla 4
                        {
                            tocaSucesso(countTeclas);
                            cmd += 4;
                        }
                        if(tecla == 0x6e) // tecla 5
                        {
                            tocaSucesso(countTeclas);
                            cmd += 5;
                        }
                        if(tecla == 0x6d) // tecla 6
                        {
                            tocaSucesso(countTeclas);
                            cmd += 6;
                        }
                        if(tecla == 0x61) // tecla 7
                        {
                            tocaSucesso(countTeclas);
                            cmd += 7;
                        }
                        if(tecla == 0x73) // tecla 8
                        {
                            tocaSucesso(countTeclas);
                            cmd += 8;
                        }
                        if(tecla == 0x64) // tecla 9
                        {
                            tocaSucesso(countTeclas);
                            cmd += 9;
                        }
                        countTeclas++;
                        tecla = 0;
                    }
//                    Serial.println(cmd);
                    if(countTeclas == 3)
                    {
                        midiSetInstrument(0, cmd);
                        toca_midi(65);
                        delay(200);
                        para_midi(65);
                        toca_midi(67);
                        delay(200);
                        para_midi(67);
                        toca_midi(69);
                        delay(200);
                        para_midi(69);
                        tecla = 0;
                        prg = 0;
                        timeProg1 = 0;
                        countTeclas = 0;
                        flagProg01 = 0; 
                        flagProg02 = 0; 
                        flagProg03 = 0; 
                    }
                }
                    
            }
        }        
        
    } else {
        timeProg1 = 0;
    }
        if(flagProg01 && flagProg02)
    {

        if(!timeProg2){ // aqui vou aumentar e diminuir o som
            timeProg2 = millis();
        }
        if(millis() - timeProg2 >= 3000)
        {
            inicio_prg();
            uint8_t countTeclas = 0;
            uint8_t vol = volume;    
            while(prg)
            {
                char tecla = teclado_personalizado.getKey();
                if(tecla != 0)
                {
//                    Serial.print("TECLA:");
//                    Serial.println(tecla);
                    if(tecla == 0x78) // tecla 1 preta
                    {
                        if (vol < 127)
                        {
                        vol += 10;
                        }
                        volume = vol;                            
                        toca_midi(65);
                        delay(200);
                        para_midi(65);
                        toca_midi(67);
                        delay(200);
                        para_midi(67);
                        countTeclas++;
                        tecla = 0;
                        countTeclas=0;
                    }
                    else //if (tecla == 'x')
                    {  
                        if (vol > 10)
                        {
                        vol -= 10;
                        }
                        volume = vol;                            
                        toca_midi(65);
                        delay(200);
                        para_midi(65);
                        toca_midi(67);
                        delay(200);
                        para_midi(67);
                        countTeclas=0;
                        tecla = 0;
                    }
                }  
                    countTeclas++;
                    delay(20);
//                    Serial.println(vol);
//                    Serial.println(countTeclas);
                    if(countTeclas >= 250)
                    {
                        volume = vol;    
                        toca_midi(65);
                        delay(200);
                        para_midi(65);
                        toca_midi(67);
                        delay(200);
                        para_midi(67);
                        toca_midi(69);
                        delay(200);
                        para_midi(69);
                        tecla = 0;
                        prg = 0;
                        timeProg2 = 0;
                        countTeclas = 0;
                        flagProg01 = 0; 
                        flagProg02 = 0; 
                        flagProg03 = 0; 
                    }                    
            }
        }        
        
    } else {
        timeProg2 = 0;
    }
}

void tocaErro()
{
    midiSetInstrument(0, 125);
    toca_midi(36);
    delay(200);
    para_midi(36);
}

void tocaSucesso(uint8_t nivel)
{
    midiSetInstrument(0, 124);
    if(nivel == 0)
    {
        toca_midi(65);
        delay(200);
        para_midi(65);
    }
    if(nivel == 1)
    {
        toca_midi(67);
        delay(200);
        para_midi(67);
    }
    if(nivel == 2)
    {
        toca_midi(69);
        delay(200);
        para_midi(69);
    }
}