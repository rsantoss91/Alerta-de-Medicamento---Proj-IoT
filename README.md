# 💡 Lembrete Inteligente de Medicamento

Este projeto é um **sistema de alerta de medicação** utilizando ESP32, display OLED, potenciômetro e buzzer.  
Ele envia notificações sonoras e via **MQTT**, com hora sincronizada automaticamente através de **NTP**.

---

## 🔬 Protótipo do Circuito

> Simulação realizada no [Wokwi](https://wokwi.com/)

![Protótipo do sistema](imagens/prototipo.png)

---

## 🛠 Materiais Necessários

- **Placa:** ESP32 DevKit C v4  
- **Display:** OLED I2C monocromático  
- **Controle de horário:** Potenciômetro  
- **Alerta sonoro:** Buzzer piezoelétrico  
- **Conexão:** Wi-Fi para MQTT e NTP

---

## ⚙️ Funcionamento do Sistema

1. **Definindo o Horário**  
   - Ajuste o potenciômetro para escolher o horário do alerta (00:00 a 23:59).

2. **Hora Atual Automática**  
   - O ESP32 consulta um servidor NTP para manter o relógio sempre correto.

3. **Monitoramento Contínuo**  
   - O microcontrolador verifica se o horário atual coincide com o configurado.

4. **Alerta de Medicação**  
   Quando o horário definido chega:  
   - O buzzer toca a cada 500ms  
   - A mensagem **“CHEGOU A HORA!”** é publicada via MQTT  
   - O OLED exibe **“ALERTA DE MEDICAMENTO!”**

5. **Visualização no Display**  
   - Horário atual  
   - Próximo horário de medicação  
   - Mensagem de alerta quando necessário

6. **MQTT**  
   - Tópicos publicados:  
     - `medicamento/horamedicamento` → horário configurado (HH:MM)  
     - `medicamento/alertamedicamento` → alerta disparado  
   - Broker: `test.mosquitto.org`  
   - Porta: `1883`  
   - Biblioteca: PubSubClient

---

## 📂 Organização dos Arquivos

```plaintext
├── sketch.ino       # Código principal
├── diagram.json     # Diagrama do circuito no Wokwi
└── libraries.txt    # Bibliotecas necessárias
```

## 🚀 Simulação no Wokwi

1. Acesse [https://wokwi.com](https://wokwi.com)  
2. Crie um novo projeto  
3. Envie os arquivos:  
   - `sketch.ino`  
   - `diagram.json`  
   - `libraries.txt`  
4. Clique em **Start Simulation**  
5. Abra o **Serial Monitor** para conferir mensagens MQTT  
6. Ajuste o **potenciômetro** e acompanhe o alerta no **OLED**

---

## 🌐 Protocolos e Comunicação

- **MQTT:** Envio de mensagens de alerta  
- **NTP:** Sincronização automática do relógio  

**Configurações:**

- Broker: `test.mosquitto.org`  
- Porta: `1883`  
- Cliente MQTT: `PubSubClient`  
- Servidor NTP: `pool.ntp.org` (GMT-3)

**Tópicos de publicação:**

| Tópico                         | Conteúdo                                      |
| -------------------------------| -------------------------------------------- |
| `medicamento/horamedicamento`  | Horário definido pelo usuário (HH:MM)        |
| `medicamento/alertamedicamento`| Mensagem de alerta quando o horário chega    |

---

## 🔄 Possíveis Extensões

- LED adicional para indicação de alerta  
- Ajuste remoto do horário via MQTT  
- Vários horários programáveis  
- Histórico de alertas  
- Integração com dashboards (Node-RED)

---

## 📄 Licença

Este projeto é distribuído sob **MIT License**. Veja `LICENSE` para mais detalhes.
