//
// Created by Liming Shao on 2018/5/10.
// 
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include "Utils.h"
#include "RTPEnc.h"
#include "Network.h"

#define CHUNK_SIZE 64 * 1024 // Buffer de 64KB para evitar exaustão de RAM

int main() {
    int res;
    const char *fileName = "../Sample.h264";
    
    // Buffer temporário para leitura em blocos
    uint8_t buffer[CHUNK_SIZE];

    RTPMuxContext rtpMuxContext;
    UDPContext udpContext = {
        .dstIp = "127.0.0.1",   // destination ip
        .dstPort = 1234         // destination port
    };

    // 1. Inicializa o Socket UDP (Agora com QoS/DSCP implementado no network.c)
    res = udpInit(&udpContext);
    if (res){
        printf("udpInit error.\n");
        return -1;
    }

    // 2. Inicializa o Contexto RTP
    initRTPMuxContext(&rtpMuxContext);

    // 3. Abertura do arquivo para leitura em fluxo (Streaming)
    FILE *fp = fopen(fileName, "rb");
    if (!fp) {
        printf("Erro ao abrir o arquivo: %s\n", fileName);
        return -1;
    }

    printf("Iniciando transmissão com QoS e Segurança...\n");

    // 4. Loop de Leitura e Envio (Resolve o problema de memória do código antigo)
    while (!feof(fp)) {
        int bytesRead = fread(buffer, 1, CHUNK_SIZE, fp);
        
        if (bytesRead > 0) {
            // rtpSendH264HEVC processará este bloco. 
            // A segurança (criptografia) será aplicada dentro desta função em rtpenc.c
            rtpSendH264HEVC(&rtpMuxContext, &udpContext, buffer, bytesRead);
        }
        
        // Pequeno delay para simular o tempo real e evitar saturação da interface
        usleep(1000); 
    }

    printf("Transmissão finalizada.\n");

    fclose(fp);
    // Nota: Não é mais necessário o free(stream) pois não usamos malloc gigante

    return 0;
}