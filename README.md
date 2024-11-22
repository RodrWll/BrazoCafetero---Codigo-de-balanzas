## Finite-state machine - Robotic Arm for Coffee

We are using two scales to measure the weight of both the coffee beans and the water. An Arduino Nano and xArm sends logic signals to control state transitions. 

## Finite-state machine Diagram
![alt text](img/state_machine.png)

```mermaid
stateDiagram
    %% Define estilos para las clases
    classDef startState fill:#f9c74f,stroke:#f9844a,stroke-width:2px,font-family:'San Francisco', sans-serif;
    classDef tareState fill:#90be6d,stroke:#43aa8b,stroke-width:2px,font-weight:bold,font-family:'San Francisco', sans-serif;
    classDef pesoState fill:#577590,stroke:#4d908e,stroke-width:2px,font-style:italic,font-family:'San Francisco', sans-serif;
    classDef apagadoState fill:#f94144,stroke:#f3722c,stroke-width:2px,font-family:'San Francisco', sans-serif;


    state "PRENDIDO\nT=0\nP=1\nST=0" as Prendido
    state "TARE\nT=1\nP=1\nST=1" as Tare
    state "PESO 1\nT=0\nP=1\nST=2\n" as Peso1
    state "PESO 2\nT=0\nP=1\nST=3\n" as Peso2
    state "PESO 3\nT=0\nP=1\nST=4\n" as Peso3
    state "PESO 4\nT=0\nP=1\nST=5\n" as Peso4
    state "APAGADO\nT=0\nP=0\nST=6" as Apagado

    [*] --> Prendido : Se espera al brazo que inicialice (prendido)
    Prendido --> Tare : Se hace el tare
    Tare --> Peso1 : chemex y filtro
    Peso1 --> Tare : G=1
    Tare --> Peso2 : café molido
    Peso2 --> Tare : G=1
    Tare --> Peso3 : agua hasta afloramiento
    Peso3 --> Tare : G=1
    Tare --> Peso4 : agua que queda
    Peso4 --> Apagado : Apagado
    Apagado --> [*]

    %% Apply styles to specific states
    Prendido:::startState
    Tare:::tareState
    Peso1:::pesoState
    Peso2:::pesoState
    Peso3:::pesoState
    Peso4:::pesoState
    Apagado:::apagadoState

```


![alt text](img/Pinout-NANO_latest.png)

## Pinout for Arduino Nano - Finite-state machine

Wiring for the Arduino Nano is as follows:

| Pin | Arduino Nano Function  | Description          | Pin Xarm Connection |
|-----|------------------------|----------------------|---------------------|
| D2  | resetButtonPin         | Reset button input   |                     |
| D3  | tare_DOUT_PIN          | Tare Xarm output     | DO5                 |
| D4  | scaleChemex_DOUT_PIN   | Signal pin 1         |                     |
| D5  | scaleChemex_SCK_PIN    | Clock signal 1       |                     |       
| D6  | dispenser_enabled      | Dispenser enabled    |                     |
| D7  | enable_DOUT_PIN        | Enable Xarm output   | DO4                 |
| D8  | scaleCoffee_DOUT_PIN   | Signal pin 2         |                     |
| D9  | scaleCoffee_SCK_PIN    | Clock signal 2       |                     |
| D10 | pinServo               | Servo control        |                     |
| D11 | messageToggleButtonPin | Message toggle button|                     |
| D12 | weigth_DIN_PIN         | Weight Xarm input    | DI1                 |
| A1  | potPin                 | Potentiometer input  |                     |
| GND | Ground                 | Common ground        | GND                 |

## Pinout for Arduino Nano - Dispensador de café

Wiring for the Arduino Nano is as follows:

| Pin | Arduino Nano Function | Description                  | Pin Connection SM    |
|-----|-----------------------|------------------------------|----------------------|
| D2  | interruptPin          | Enable pin from state machine|         ??¿¿         |
| D3  | PWMA                  | Motor speed control          |                      |
| D5  | pinServo1             | Servo control pin            |                      |
| D10 | AIN1                  | Motor direction control 1    |                      |
| D11 | AIN2                  | Motor direction control 2    |                      |

## PCB Schematic - Cafetero

![image](https://github.com/user-attachments/assets/9214e00e-cf4c-42d6-ad64-f00602fbfc66)

**
| A2  | potentiometer         | Potentiometer input          |                      |
| GND | Ground                | Common ground                |             GND      |
