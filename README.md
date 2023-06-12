# Proyecto-Emebebidos

DATOS:
    Card UID: C7 45 85 01
    Card SAK: 08
    PICC type: MIFARE 1KB

    Card UID: C7 45 85 01
    Card SAK: 08
    PICC type: MIFARE 1KB

Ficha
Card UID:  C302 F302 2002 9B02PICC Type: MIFARE 1KB 
C3 F3 20 9B
Tarjeta:
Card UID:  C702 4502 8502 102PICC Type: MIFARE 1KB
C7 45 85 01


TO DO:

En la funcion commandSetFeederTime(const char charReceived) la duracion en fija, falta implementar el parser para poner segundos minutos y horas

Deberia cambiar oara que feeder set feede status arranque en modo apagado(deberia gregar un nuevo modo el feeder)

No deberia poder entrar a cambaiar el tiempo del feeder cuando no estoy en modo TIME MODE

Implementar en feeder update como cambian los motores con cada modo y agregar a pcserial botones para controlar en modo manual los motores