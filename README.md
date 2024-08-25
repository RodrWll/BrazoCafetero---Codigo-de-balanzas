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




## Pinout for Arduino Nano

Wiring for the Arduino Nano is as follows:

| Pin | Function       | Description          |
|-----|----------------|----------------------|
| D4  | scaleCoffee_DOUT_PIN   |          |
| D5  | scaleCoffee_SCK_PIN  |           |
| D8  | scaleChemex_DOUT_PIN    |   |
| D9  | scaleChemex_SCK_PIN  |         |
| D8  | xArm_DOUT_PIN  |         |
| GND | Ground         | Ground connection    |


