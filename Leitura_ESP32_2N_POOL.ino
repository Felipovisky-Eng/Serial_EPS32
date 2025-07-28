#include <Arduino.h>

// --- Variáveis globais ---
#define N 1024
#define TOTAL_AMOSTRAS 51200

typedef struct {
  uint32_t tempo;
  uint16_t leitura;
} amostra_t;

QueueHandle_t fila;

// --- Função setup ---
void setup() {
  Serial.begin(1500000);
  Serial.flush();
  delay(100);
  Serial.println("READY");

  analogReadResolution(10);

  fila = xQueueCreate(2 * N, sizeof(amostra_t)); // Cria fila com 2*N amostras

  // Cria as tasks
  xTaskCreatePinnedToCore(TaskADC, "ADC", 4096, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(TaskSerial, "Serial", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // Nada
}

// --- Task ADC ---
void TaskADC(void *pvParameters) {
  amostra_t amostra;

  for (uint32_t i = 0; i < TOTAL_AMOSTRAS; i++) {
    amostra.tempo = micros();
    amostra.leitura = analogRead(34);

    // --- Controle opcional (descomente se necessário) ---
    // delayMicroseconds(5); // Se você quiser "relaxar" o ADC

    // Envia para a fila (bloqueia se estiver cheia)
    xQueueSend(fila, &amostra, portMAX_DELAY);

    taskYIELD(); // Cede o controle
  }

  vTaskDelete(NULL); // Encerra a TaskADC
}

// --- Task Serial ---
void TaskSerial(void *pvParameters) {
  amostra_t amostra;
  uint16_t pacote_num = 0;
  uint16_t cont = 0;

  while (1) {
    // Recebe amostra da fila (espera caso vazia)
    if (xQueueReceive(fila, &amostra, portMAX_DELAY)) {

      if (cont == 0) {
        Serial.write("DATA", 4);
        Serial.write((uint8_t *)&pacote_num, 2);
      }

      Serial.write((uint8_t *)&amostra.tempo, sizeof(amostra.tempo));
      Serial.write((uint8_t *)&amostra.leitura, sizeof(amostra.leitura));

      cont++;

      if (cont >= N) {
        cont = 0;
        pacote_num++;
      }

      taskYIELD(); // Cede o controle
    }
  }

  Serial.println("FIM");
  vTaskDelete(NULL); // Encerra a TaskSerial
}
