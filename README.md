# Sistema de Aquisição de Dados com ESP32 (10 kHz via Serial)

Este projeto consiste em um sistema de aquisição de dados com ESP32, com taxa de amostragem estável de **10 kHz**, enviando os dados diretamente via **porta serial** em formato binário. Todo o processamento, visualização e análise de dados (FFT, autocorrelação, filtros) é realizado em **Python**.

---

## 🧠 Objetivo

Desenvolver um sistema simples, confiável e de alta frequência de aquisição de dados analógicos utilizando o ESP32 e a porta serial para transmissão, com análise posterior em Python. A meta era atingir **10.000 amostras por segundo com precisão e estabilidade**.

---

## ⚙️ Estrutura do Projeto

```
📁 Projeto/
👉📁 scripts/
    👉 aquisicao_serial.py         # Recebe dados via serial e salva em .csv
    👉 analise_fft_filtro.py       # FFT, autocorrelação, filtro e gráficos
    👉 analise_jitter.py           # Geração de gráficos de jitter e tempo
👉📁 dados/
    👉 leitura_teste_10khz.csv     # Dados brutos da ESP32
    👉 leitura_teste_completa.txtx # Backup / análise paralela
👉 README.md
👉 esp32_serial_10khz.ino          # Código Arduino/C++ para ESP32
```

---

## 📡 Funcionamento da Comunicação Serial

Cada amostra consiste em:

- **4 bytes** para `micros()` (tempo)
- **2 bytes** para leitura ADC (`analogRead(34)`)

Transmissão total: **6 bytes por amostra**

Taxa de baudrate: **1.500.000** (1.5 Mbaud)

### Cálculo de taxa máxima:

```text
1.5 Mbps / 8 = 187.5 kB/s
187.5 kB/s / 6 bytes = 31.250 amostras/s (limite teórico)
```

Com margem e estabilidade: **10.000 amostras/s (10 kHz)**

---

## ✅ Código C++ para ESP32 (Última versão testada com sucesso)

```cpp
void setup() {
  Serial.begin(1500000);
  delay(100);
  analogReadResolution(10);

  // Aquecimento do ADC
  for (int i = 0; i < 10000; i++) {
    analogRead(34);
    delayMicroseconds(50);
  }

  delay(1000);
}

void loop() {
  for (int i = 0; i < 52100; i++) {
    uint32_t tempo = micros();
    uint16_t leitura = analogRead(34);
    Serial.write((uint8_t*)&tempo, 4);
    Serial.write((uint8_t*)&leitura, 2);
    delayMicroseconds(20);  // Mantém taxa de ~10 kHz
  }

  while (true); // Fim da aquisição
}
```

---

## 🐍 Scripts em Python

### aquisicao\_serial.py

- Recebe os dados pela porta serial
- Salva no arquivo `leitura_teste_10khz.csv`
- Calcula taxa real de amostragem
- Não adiciona cabeçalhos ao CSV

### analise\_fft\_filtro.py

- Carrega o CSV
- Realiza FFT, aplica filtros (FIR/IIR)
- Calcula autocorrelação
- Identifica frequências dominantes
- Exibe gráficos de espectro e sinal

### analise\_jitter.py

- Calcula jitter (variação entre tempos)
- Gera gráfico do tempo acumulado (esperado: rampa)
- Gera gráfico do jitter amostra a amostra

---

## 📊 Exemplo de Resultados

### Frequência de amostragem real:

```bash
Frequência de amostragem: 9999.2 Hz
```

### Gráficos:

- **tempo\_amostras.svg**: tempo acumulado (esperado: rampa contínua)
- **jitter.svg**: flutuação no intervalo de tempo entre amostras
- **fft.svg**: espectro de frequência do sinal capturado

---

## 📀 Arquivos incluídos

- `.csv`: dados brutos de leitura ADC com tempo
- `.txtx`: versão alternativa usada para validação e testes
- `.py`: scripts de análise
- `.ino`: código C++ da ESP32

---

## 📊 Próximos passos (opcional)

- Adição de header por pacote com checksum
- Compressão dos dados para maior taxa
- Uso de SD card como buffer primário com DMA
- Geração de relatório automático com LaTeX (Overleaf)

---

## 📜 Licença

Este projeto é distribuído sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

