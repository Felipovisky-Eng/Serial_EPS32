
# 📡 Sistema de Aquisição de Dados com ESP32 via Serial (10 kHz)

Este projeto implementa um sistema simples e confiável de aquisição de dados analógicos usando a ESP32. Os dados são transmitidos diretamente para um computador via porta serial em uma taxa de **10 kHz**, com precisão garantida por controle de tempo e envio binário.

---

## 🛠️ Objetivo

Realizar leituras analógicas a 10 kHz e enviar os dados diretamente para um computador com mínima latência, alta estabilidade e baixo uso de recursos, evitando buffers complexos ou uso de cartões SD.

---

## ⚙️ Como Funciona

- A ESP32 realiza leituras do pino ADC (`GPIO34`) utilizando `analogRead()`.
- Cada leitura é marcada com o tempo do sistema obtido por `micros()`.
- Os dados (tempo e leitura) são enviados diretamente na forma binária pela serial:
  - 4 bytes para o tempo (`uint32_t`)
  - 2 bytes para a leitura (`uint16_t`)
- Um `delayMicroseconds(20)` garante que a taxa de amostragem fique em torno de 10.000 amostras por segundo.
- O código Python recebe os dados, salva em `.csv` e calcula a taxa real de amostragem e o jitter.

---

## 🧪 Requisitos

- ESP32 (modelo WROOM32 testado)
- Sensor ou sinal conectado ao pino `GPIO34` (ADC1)
- Conexão serial com o computador (via cabo USB)
- Baud rate: **1.5 Mbaud**

---

## 🔩 Código C++ para ESP32

```cpp
void setup() {
  Serial.begin(1500000); // Comunicação serial rápida
  delay(1000); // Aguarda estabilidade
}

void loop() {
  for (int i = 0; i < 52100; i++) {
    uint32_t tempo = micros();            // Tempo da leitura
    uint16_t leitura = analogRead(34);    // Leitura do sinal analógico
    Serial.write((uint8_t*)&tempo, 4);    // Envia tempo (4 bytes)
    Serial.write((uint8_t*)&leitura, 2);  // Envia leitura (2 bytes)
    delayMicroseconds(20);                // Garante taxa de ~10 kHz
  }

  while (true); // Encerramento após enviar todas as leituras
}
```

---

## 🐍 Código Python (receptor)

Veja `receptor.py` neste repositório para:
- Receber os dados via serial.
- Armazenar em `dados.csv` (sem cabeçalho).
- Calcular a frequência de amostragem.
- Exibir gráfico de jitter (variação de tempo entre amostras).
- Plotar gráfico de tempo total por amostra.

---

## 🧮 Cálculos de Tempo

- `analogRead() ≈ 40 µs`
- `Serial.write(6 bytes) @ 1.5 Mbaud ≈ 32 µs`
- `delayMicroseconds(20)` completa o ciclo.

**Total ≈ 92 µs → Frequência estimada: ~10.8 kHz**

Com a sobrecarga do sistema, atinge com precisão **9999 Hz**, como medido.

---

## 📈 Exemplo de saída

```plaintext
Leituras recebidas: 52100
Tempo total: 5.21 s
Frequência real: 9999.1 Hz
```

---

## 📎 Arquivos incluídos

- `main.ino` – Código da ESP32
- `receptor.py` – Código de recepção em Python
- `dados.csv` – Arquivo gerado com os dados (tempo, leitura)
- `tempo_amostras.svg` – Gráfico da evolução do tempo
- `jitter.svg` – Gráfico da variação entre tempos

---

## 🔒 Licença

Este projeto está sob a licença MIT. Sinta-se à vontade para usar, modificar e compartilhar.

---

## 🤝 Colaboradores

Desenvolvido por **Luis Felipe Pereira Ramos**, com apoio técnico para análise de sinal, otimização de tempo real e estruturação de protocolos de transmissão eficiente.
