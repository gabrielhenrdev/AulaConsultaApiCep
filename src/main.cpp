
#include <Arduino.h>
#include <WiFi.h>
#include <ezTime.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Prototipo de funções
void conexaoWIFI();
void reconexaoWIFI();
String consultaApicep(String cep);

const char *SSID = "SENAI EDUCACIONAL";
const char *SENHA = "info@134";

const unsigned long tempoConexao = 10000;
const unsigned long tempoReconexao = 5000;
unsigned long tempoultimaConexao = 0;

const String enderecoApi = "https://brasilapi.com.br/api/cep/v2/";

void setup() {

    Serial.begin(115200);

    conexaoWIFI();

}

void loop() {

    reconexaoWIFI();

    Serial.println();
    Serial.println("Digite um CEP:");

    String cep = "";

    while (true) {

        reconexaoWIFI();

        if (Serial.available() > 0) {

            char caractere = Serial.read();

            if (caractere == '\n') {

                Serial.println();
                break;

            }

            if (caractere != '\r') {

                cep += caractere;

                Serial.print(caractere);

            }

        }

    }

    cep.trim();

    if (cep.length() == 8) {

        int quantidadeNumeros = 0;

        for (int i = 0; i < 8; i++) {

            if (cep[i] >= '0' && cep[i] <= '9') {

                quantidadeNumeros++;

            }

        }

        if (quantidadeNumeros == 8) {

            Serial.println("CEP valido!");

            if (WiFi.status() == WL_CONNECTED) {

                String respostaapi = consultaApicep(cep);

                if (respostaapi != "") {

                    JsonDocument doc;

                    DeserializationError erro = deserializeJson(doc, respostaapi);

                    if (!erro) {

                        String rua = doc["street"];
                        String bairro = doc["neighborhood"];
                        String cidade = doc["city"];
                        String estado = doc["state"];

                        Serial.println();
                        Serial.println("Dados do endereco");
                        Serial.println("--------------");

                        Serial.println("CEP: " + cep);
                        Serial.println("Rua: " + rua);
                        Serial.println("Bairro: " + bairro);
                        Serial.println("Cidade: " + cidade);
                        Serial.println("Estado: " + estado);

                    }
                    else {

                        Serial.println("Erro ao interpretar o JSON");

                    }

                }

            }
            else {

                Serial.println("Sem conexao com a internet.");

            }

        }
        else {

            Serial.println("CEP invalido! Digite somente numeros.");

        }

    }
    else {

        Serial.println("CEP invalido! Digite exatamente 8 numeros.");

    }

}

void conexaoWIFI() {

    Serial.printf("Conectando ao WiFi: %s\n", SSID);

    WiFi.begin(SSID, SENHA);

    unsigned long tempoWifi = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - tempoWifi < tempoConexao) {

        Serial.print("-");
        delay(500);

    }

    if (WiFi.status() == WL_CONNECTED) {

        Serial.println("\nConectado ao WiFi!");

        Serial.print("Endereco IP: ");
        Serial.println(WiFi.localIP());

    }
    else {

        Serial.println("\nFalha ao se conectar.");

    }

}

void reconexaoWIFI() {

    unsigned long tempoAtual = millis();

    if (tempoAtual - tempoultimaConexao >= tempoReconexao) {

        tempoultimaConexao = tempoAtual;

        if (WiFi.status() != WL_CONNECTED) {

            Serial.println("Voce perdeu a conexao!");

            conexaoWIFI();

            if (WiFi.status() == WL_CONNECTED) {

                waitForSync();

            }

        }

    }

}

String consultaApicep(String cep) {

    HTTPClient http;

    http.begin(enderecoApi + cep);

    int codigohttp = http.GET();

    String resposta = "";

    if (codigohttp == 200) {

        resposta = http.getString();

    }
    else if (codigohttp == 404) {

        Serial.println("CEP nao encontrado. Cod: " + String(codigohttp));

    }
    else {

        Serial.println("Erro. Codigo: " + String(codigohttp));

    }

    http.end();

    return resposta;

}
