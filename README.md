# DCF-Uhr-mit-LCD
Einfache DCF77-Uhr mit Character-LCD auf einem ATmega16

Programmiert im AVR Studio (mit AVR-GCC)
Hardware:
STK500
ATmega16
16MHz Quarz
20×4 Zeichen LCD (HD44780 kompatibel)
DCF77-Empfangsmodul von Conrad (641138) (PullUp-Widerstand 10k zwischen
Anschluß 2 und 4)

Die LCD-Ansteuerung habe ich aus dem Tutorial übernommen, vielen Dank an
den Autor an dieser Stelle
Ansteuerung Hardware/Software entsprechend dem AVR-GCC-Tutorial
http://www.mikrocontroller.net/articles/AVR-GCC-Tu…

Nun zu meinem Programm

Abtastung erfolgt mit einem 1ms Trigger.

Die Synchronisation zum DCF-Signal erfolgt durch einen High-Pegel
zwischen 1750-1950ms, das ist das 59Bit einer Minute, welches nicht
abgesenkt wird.

Danach beginnt die Auswertung der Datenbits, dabei habe ich folgende
Werte angenommen
Pegel-Absenkung von 60-140ms entspricht einer logischen 0
Pegel-Absenkung von 160-240ms entspricht einer logischen 1
keine Absenkung für 1750-1950ms entspricht dem Beginn einer neuen Minute

Abbruchkriterien (bzw. Start einer neuen Synchronisation)
…wenn mehr oder weniger als 59Bits pro Minute erkannt werden.
…Pegel-Zeiten die außerhalb der oben genannten Toleranzen (für
0/1/neue Minute) liegen
…Überprüfung der Minuten-, Stunden- und Datums-Parität

Über einen externen Quarz wird ein 1ms Takt erzeugt, damit läuft die
Uhrzeit/das Datum auch weiter, wenn keine korrekten DCF-Daten empfangen
werden.

Abgleich zwischen der internen Uhr und den DCF-Daten bei korrektem
Empfang (hier plane ich noch eine Plausibilitätsüberprüfung einzubauen)

Auf das LCD werden folgende Daten ausgegeben:
Wochentag (Mo/Di/Mi/Do/Fr/Sa/So)
Datum (dd.mm.yyyy) (Meine Lösung für die Jahreszahl ist noch etwas
rudimentär, ob in 90 Jahren noch jemand DCF77-Uhren benutzt? 😉 )
Zeitzone (MEZ/MESZ)
Zeit (hh:mm:ss)
Status der Synchronisation (?/!)

ToDo’s:
Behandlung der Schaltsekunde (DCF-Auswertung „case 19“)
Behandlung der angekündigten Zeitumstellung (DCF-Auswertung „case 16“)
Anpassung für die zweistelligen Jahreszahlen

Geplante Erweiterungen:
Grafik-LCD (320×240) damit ich später noch andere Dinge anzeigen kann
