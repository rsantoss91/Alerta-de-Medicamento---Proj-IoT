#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ------------------- Configuracoes do display -------------------
#define LARGURA_TELA 128
#define ALTURA_TELA 64
#define RESET_OLED -1
Adafruit_SSD1306 tela(LARGURA_TELA, ALTURA_TELA, &Wire, RESET_OLED);

// ------------------- Pinos -------------------
#define BUZZER 25
#define POTENCIOMETRO 34

// ------------------- Rede e MQTT -------------------
const char* SSID = "Wokwi-GUEST";
const char* SENHA = "";
const char* SERVIDOR_MQTT = "test.mosquitto.org";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WiFiUDP udp;
NTPClient horario(udp, "pool.ntp.org", -10800, 60000); // GMT-3

// ------------------- Variaveis de tempo -------------------
int horaAlerta = 14;
int minutoAlerta = 30;
int horaAnterior = -1;
int minutoAnterior = -1;

unsigned long ultimoBuzzer = 0;
bool estadoBuzzer = false;
const unsigned int intervaloBuzzer = 500;

unsigned long ultimoEnvio = 0;
const unsigned long intervaloEnvio = 1000;

// ------------------- Funcoes -------------------
void conectarWiFi() {
  Serial.print("Conectando a rede: ");
  Serial.println(SSID);
  WiFi.begin(SSID, SENHA);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWi-Fi conectado!");
}

void reconectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (mqttClient.connect("ESP32_Lembrete")) {
      Serial.println("Conectado ao broker!");
    } else {
      Serial.print("Erro, rc=");
      Serial.println(mqttClient.state());
      delay(3000);
    }
  }
}

void publicarMensagem(const char* topico, const char* mensagem) {
  if (!mqttClient.connected()) reconectarMQTT();
  mqttClient.publish(topico, mensagem);
  Serial.print("Mensagem publicada em ");
  Serial.print(topico);
  Serial.print(": ");
  Serial.println(mensagem);
}

// ------------------- Setup -------------------
void setup() {
  Serial.begin(115200);

  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  if (!tela.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Erro ao inicializar o OLED"));
    while (true);
  }

  tela.clearDisplay();
  tela.setTextSize(1);
  tela.setTextColor(SSD1306_WHITE);
  tela.setCursor(10, 25);
  tela.println("Sistema de Lembrete");
  tela.setCursor(10, 40);
  tela.println("Iniciando...");
  tela.display();
  delay(2000);

  conectarWiFi();
  mqttClient.setServer(SERVIDOR_MQTT, 1883);
  horario.begin();
  horario.update();
}

// ------------------- Loop -------------------
void loop() {
  if (!mqttClient.connected()) reconectarMQTT();
  mqttClient.loop();
  horario.update();

  int horaAtual = horario.getHours();
  int minutoAtual = horario.getMinutes();

  // Ajusta horario do alerta via potenciometro
  int leituraPot = analogRead(POTENCIOMETRO);
  int minutosTotais = map(leituraPot, 0, 4095, 0, 1439);
  horaAlerta = minutosTotais / 60;
  minutoAlerta = minutosTotais % 60;

  // Publica horario de alerta se mudou ou a cada segundo
  if (horaAlerta != horaAnterior || minutoAlerta != minutoAnterior) {
    horaAnterior = horaAlerta;
    minutoAnterior = minutoAlerta;
    char mensagem[10];
    sprintf(mensagem, "%02d:%02d", horaAlerta, minutoAlerta);
    publicarMensagem("medicamento/horamedicamento", mensagem);
  } else if (millis() - ultimoEnvio >= intervaloEnvio) {
    char mensagem[10];
    sprintf(mensagem, "%02d:%02d", horaAlerta, minutoAlerta);
    publicarMensagem("medicamento/horamedicamento", mensagem);
    ultimoEnvio = millis();
  }

  // Atualiza display
  tela.clearDisplay();
  tela.setTextSize(1);
  tela.setCursor(0, 0);
  tela.printf("Horario agora: %02d:%02d", horaAtual, minutoAtual);

  // Verifica se e hora de alertar
  if (horaAtual == horaAlerta && minutoAtual == minutoAlerta) {
    if (millis() - ultimoBuzzer >= intervaloBuzzer) {
      estadoBuzzer = !estadoBuzzer;
      if (estadoBuzzer) {
        tone(BUZZER, 1000);
        publicarMensagem("medicamento/alertamedicamento", "CHEGOU A HORA!");
      } else {
        noTone(BUZZER);
      }
      ultimoBuzzer = millis();
    }

    tela.setCursor(0, 5);
    tela.println("ALERTA DE MEDICAMENTO");
    tela.setCursor(0, 40);
    tela.println("Nao esqueca de tomar!");
  } else {
    noTone(BUZZER);
    tela.setCursor(0, 25);
    tela.println("Proximo lembrete:");
    tela.setCursor(0, 40);
    tela.printf("%02d:%02d", horaAlerta, minutoAlerta);
  }

  tela.display();
  delay(100);
}