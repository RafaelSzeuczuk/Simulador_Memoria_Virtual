#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <sstream>
using namespace std;

string disk();
const int TLB_SIZE = 3;
string tlb[TLB_SIZE][2]; 

void init_tlb()
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        tlb[i][0] = ""; // Endereço Virtual
        tlb[i][1] = ""; // Endereço Físico
    }
}

void update_tlb(string endereco_virtual, string endereco_fisico)
{
    static int tlb_indice;
    tlb[tlb_indice][0] = endereco_virtual;
    tlb[tlb_indice][1] = endereco_fisico;
    tlb_indice = (tlb_indice + 1) % TLB_SIZE; // Circular buffer
}

string buscar_na_tlb(string endereco_virtual)
{
    for (int i = 0; i < TLB_SIZE; i++)
    {
        if (tlb[i][0] == endereco_virtual)
        {
            return tlb[i][1];
        }
    }
    return "";
}

void alocar_pagina(int numero_pagina, int tamanho_virtual, string end_virtuais_pg[][2])
{
    for (int num_end_vir = 0, i = 0; num_end_vir < tamanho_virtual; num_end_vir += 10, i++)
    {
        int num_end_virtual = numero_pagina * 100 + num_end_vir;
        ostringstream oss;
        oss << numero_pagina << "vx" << setfill('0') << setw(5) << num_end_virtual;
        end_virtuais_pg[i][0] = oss.str();
        end_virtuais_pg[i][1] = disk();    // Armazena o conteúdo
    }
}

void gerar_end_fisicos(string end_fisicos[][2], int fisico_tamanho)
{
    int indice = 0;
    for (int i = 0; i < fisico_tamanho / 24; i++)
    {
        for (int k = 0; k < 24; k++)
        {
            ostringstream oss;
            oss << i << "fx" << setw(5) << setfill('0') << k;
            end_fisicos[indice][0] = oss.str(); // Armazena o endereço físico
            end_fisicos[indice][1] = "";        // Inicializa o conteúdo como vazio
            indice++;
        }
    }
}

void preenche_end_fisicos(string end_fisicos[][2], string pagina_0[][2], string pagina_1[][2])
{
    for (int i = 0; i < 14; i++)
    {
        end_fisicos[i][1] = "Disk";
    }
    for (int i = 14; i < 24; i++)
    {
        end_fisicos[i][1] = pagina_0[i - 14][0];
    }
    for (int i = 24; i < 38; i++)
    {
        end_fisicos[i][1] = "Disk";
    }
    for (int i = 38; i < 48; i++)
    {
        end_fisicos[i][1] = pagina_1[i - 38][0];
    }
}

void imprimir_tabela(string end_fisicos[][2], int fisico_tamanho)
{
    cout << left << setw(10) << "Índice" << setw(20) << "Memória Física" << "Conteúdo" << endl;
    cout << string(60, '-') << endl;
    for (int i = 0; i < fisico_tamanho; i++)
    {
        cout << left << setw(10) << i << setw(20) << end_fisicos[i][0] << end_fisicos[i][1] << endl;
    }
}

void traduzir_endereco_virtual(string end_virtuais_pg_0[][2], string end_virtuais_pg_1[][2], string end_fisicos[][2], int tamanho_virtual)
{
    string pesquisa;
    cout << "Digite o endereço que deseja pesquisar: ";
    cin >> pesquisa;

    bool encontrado = false;

    string endereco_fisico = buscar_na_tlb(pesquisa);
    if (endereco_fisico != "")
    {
        cout << "TLB HIT! " << endl
             << "Endereço fisico correspondente: " << endereco_fisico<<endl;

        for (int i = 0; i < tamanho_virtual; i++)
        {
            if (pesquisa == end_virtuais_pg_0[i][0])
            {
                cout << "Conteudo do Endereço virtual: " << end_virtuais_pg_0[i][1] << endl;
                encontrado = true;
                break;
            }
            if (pesquisa == end_virtuais_pg_1[i][0])
            {
                cout << "Conteudo do Endereço virtual: " << end_virtuais_pg_1[i][1] << endl;
                encontrado = true;
                break;
            }
        }

        return;
    }
    cout << "TLB MISS!" << endl;

    for (int i = 0; i < tamanho_virtual; i++)
    {
        if (pesquisa == end_virtuais_pg_0[i][0])
        {
            endereco_fisico = end_fisicos[i + 14][0];
            update_tlb(pesquisa, endereco_fisico);
            cout << "Endereço virtual, local na memória física: " << endereco_fisico << "\nConteúdo: " << end_virtuais_pg_0[i][1] << endl;
            encontrado = true;
            break;
        }
        if (pesquisa == end_virtuais_pg_1[i][0])
        {
            endereco_fisico = end_fisicos[i + 38][0];
            update_tlb(pesquisa, endereco_fisico);
            cout << "Endereço virtual, local na memória física: " << endereco_fisico << "\nConteúdo: " << end_virtuais_pg_1[i][1] << endl;
            encontrado = true;
            break;
        }
    }

    for (int h = 0; h < 48; h++)
    {
        if (pesquisa == end_fisicos[h][0])
        {
            cout << "Endereço físico encontrado!\n";
            if (end_fisicos[h][1] == "Disk")
            {
                cout << "Conteúdo: Disk" << endl;
            }
            else
            {
                cout << "Conteúdo: " << end_fisicos[h][1] << endl;
                for (int i = 0; i < tamanho_virtual; i++)
                {
                    if (end_fisicos[h][1] == end_virtuais_pg_0[i][0])
                    {
                        cout << "Conteúdo da memória virtual: " << end_virtuais_pg_0[i][1] << endl;
                    }
                    if (end_fisicos[h][1] == end_virtuais_pg_1[i][0])
                    {
                        cout << "Conteúdo da memória virtual: " << end_virtuais_pg_1[i][1] << endl;
                    }
                }
            }
            encontrado = true;
            break;
        }
    }
    if (!encontrado)
    {
        cout << "Endereço não encontrado." << endl;
    }
}

int main()
{
    srand(1);
    init_tlb();
    int tamanho_virtual = 10;    
    int fisico_tamanho = 48; 

    string end_fisicos[fisico_tamanho][2];
    gerar_end_fisicos(end_fisicos, fisico_tamanho);

    string end_virtuais_pg_0[tamanho_virtual][2];
    alocar_pagina(0, tamanho_virtual * 10, end_virtuais_pg_0);
    cout << "Endereços Virtuais da Página 0:" << endl;
    for (int g = 0; g < tamanho_virtual; g++)
    {
        cout << end_virtuais_pg_0[g][0] << endl;
    }
    cout << endl;

    string end_virtuais_pg_1[tamanho_virtual][2];
    alocar_pagina(1, tamanho_virtual * 10, end_virtuais_pg_1);
    cout << "Endereços Virtuais da Página 1:" << endl;
    for (int g = 0; g < tamanho_virtual; g++)
    {
        cout << end_virtuais_pg_1[g][0] << endl;
    }
    cout << endl;

    preenche_end_fisicos(end_fisicos, end_virtuais_pg_0, end_virtuais_pg_1);
    imprimir_tabela(end_fisicos, fisico_tamanho);
    char denovo;
    do
    {
        traduzir_endereco_virtual(end_virtuais_pg_0, end_virtuais_pg_1, end_fisicos, tamanho_virtual);
        cout << "Deseja buscar um novo endereço [y para sim][n para nao]? ";
        cin >> denovo;
    } while (denovo == 'y');

    return 0;
}
string disk()
{

    int aleat = (rand() % 5);
    switch (aleat)
    {
    case 0:
        return (R"(
            _____/\\\\\\\\\________/\\\\\\\\\\\__________/\\\\\\\\\__/\\\\\\\\\\\__/\\\\\\\\\\\_        
             ___/\\\\\\\\\\\\\____/\\\/////////\\\_____/\\\////////__\/////\\\///__\/////\\\///__       
              __/\\\/////////\\\__\//\\\______\///____/\\\/_______________\/\\\_________\/\\\_____      
               _\/\\\_______\/\\\___\////\\\__________/\\\_________________\/\\\_________\/\\\_____     
                _\/\\\\\\\\\\\\\\\______\////\\\______\/\\\_________________\/\\\_________\/\\\_____    
                 _\/\\\/////////\\\_________\////\\\___\//\\\________________\/\\\_________\/\\\_____   
                  _\/\\\_______\/\\\__/\\\______\//\\\___\///\\\______________\/\\\_________\/\\\_____  
                   _\/\\\_______\/\\\_\///\\\\\\\\\\\/______\////\\\\\\\\\__/\\\\\\\\\\\__/\\\\\\\\\\\_ 
                    _\///________\///____\///////////___________\/////////__\///////////__\///////////__        
        )");
        break;
    case 1:
        return (R"(  
             ________  ________  ________  ___  ___  ___  _________  _______  _________  ___  ___  ________  ________     
            |\   __  \|\   __  \|\   __  \|\  \|\  \|\  \|\___   ___\\  ___ \|\___   ___\\  \|\  \|\   __  \|\   __  \    
            \ \  \|\  \ \  \|\  \ \  \|\  \ \  \\\  \ \  \|___ \  \_\ \   __/\|___ \  \_\ \  \\\  \ \  \|\  \ \  \|\  \   
             \ \   __  \ \   _  _\ \  \\\  \ \  \\\  \ \  \   \ \  \ \ \  \_|/__  \ \  \ \ \  \\\  \ \   _  _\ \   __  \  
              \ \  \ \  \ \  \\  \\ \  \\\  \ \  \\\  \ \  \   \ \  \ \ \  \_|\ \  \ \  \ \ \  \\\  \ \  \\  \\ \  \ \  \ 
               \ \__\ \__\ \__\\ _\\ \_____  \ \_______\ \__\   \ \__\ \ \_______\  \ \__\ \ \_______\ \__\\ _\\ \__\ \__\
                \|__|\|__|\|__|\|__|\|___| \__\|_______|\|__|    \|__|  \|_______|   \|__|  \|_______|\|__|\|__|\|__|\|__|
                                          \|__|                                                                           
                                                                                                
            )");
        break;
    case 2:
        return (R"(  
             ██████╗ ██████╗ ███╗   ███╗██████╗ ██╗   ██╗████████╗ █████╗  ██████╗ █████╗  ██████╗ 
            ██╔════╝██╔═══██╗████╗ ████║██╔══██╗██║   ██║╚══██╔══╝██╔══██╗██╔════╝██╔══██╗██╔═══██╗
            ██║     ██║   ██║██╔████╔██║██████╔╝██║   ██║   ██║   ███████║██║     ███████║██║   ██║
            ██║     ██║   ██║██║╚██╔╝██║██╔═══╝ ██║   ██║   ██║   ██╔══██║██║     ██╔══██║██║   ██║
            ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║     ╚██████╔╝   ██║   ██║  ██║╚██████╗██║  ██║╚██████╔╝
             ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝      ╚═════╝    ╚═╝   ╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝ ╚═════╝ )");
        break;
    case 3:
        return (R"(
            /$$      /$$                                             /$$          
            | $$$    /$$$                                            |__/          
            | $$$$  /$$$$  /$$$$$$  /$$$$$$/$$$$   /$$$$$$   /$$$$$$  /$$  /$$$$$$ 
            | $$ $$/$$ $$ /$$__  $$| $$_  $$_  $$ /$$__  $$ /$$__  $$| $$ |____  $$
            | $$  $$$| $$| $$$$$$$$| $$ \ $$ \ $$| $$  \ $$| $$  \__/| $$  /$$$$$$$
            | $$\  $ | $$| $$_____/| $$ | $$ | $$| $$  | $$| $$      | $$ /$$__  $$
            | $$ \/  | $$|  $$$$$$$| $$ | $$ | $$|  $$$$$$/| $$      | $$|  $$$$$$$
            |__/     |__/ \_______/|__/ |__/ |__/ \______/ |__/      |__/ \_______/
            )");
        break;
    case 4:
        return (R"(
             /$$$$$$$  /$$$$$$  /$$$$$$  /$$   /$$
            | $$__  $$|_  $$_/ /$$__  $$| $$  /$$/
            | $$  \ $$  | $$  | $$  \__/| $$ /$$/ 
            | $$  | $$  | $$  |  $$$$$$ | $$$$$/  
            | $$  | $$  | $$   \____  $$| $$  $$  
            | $$  | $$  | $$   /$$  \ $$| $$\  $$ 
            | $$$$$$$/ /$$$$$$|  $$$$$$/| $$ \  $$
            |_______/ |______/ \______/ |__/  \__/)");
        break;
    }
}