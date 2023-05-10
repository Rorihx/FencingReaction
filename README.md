# FencingReaction

Aufbau:
- Verbinde den Ethernet-Anschluss mit der Anzeige
  Hierüber wird auch das Steuer-Gerät mit Spannung versorgt.
  Eine zusätzliche Spannungsversorgung ist normalerweise nicht nötig 
- Schließe den Degen (über eine Rolle) an den Degenanschluß an.
- Schließe LED an den Cinch-Anschluss an.
- Schließe, wenn gewünscht, das Target an den Cinch-Anschluss an.

Einstellungen:

- Reaktionszeit

Anhand des Drehknopfs wird die Reaktionszeit zwischen 1 und 10 Sekunden eingestellt.
Diese ist allerdings nicht exakt gleich, sondern wird durch einen Zufallswert verändert,
sodass die Zeit um 1 Sekunde schwanken kann.

- Starttaster: siehe unten


- Target

-- Oben: es wird lediglich das Schliessen der Spitze berücksichtigt

-- Mitte: nicht definiert

-- Unten: das Target muss angeschlossen sein. Nur Treffer auf dem Target sind gültig


- Modus:
-- Oben - Einzelmodus (bei geringem Besuch) - es muss jedesmal neu START gedrückt werden
-- Mitte - Automatikmodus (bei starkem Besuch) - nach 5 Sekunden ist die Anlage von selbst wieder scharf
-- Unten - reserviert (derzeit wie Automatikmodus)

Bedienung

Wird die Anlage nicht benutzt, erscheint der Text "SGS Fechten" als Laufschrift.
(Da es sich um eine 7-Segment-Anzeige handelt, ist der Schriftzug gewöhnungsbedürftig.)

Die Reaktionszeitmessung läuft wie folgt: 

Sobald eine Lampe leuchtet, läuft die Zeit.
Dann soll der Benutzer schnellstmöglich den Degen auf das Ziel stoßen, 
und dadurch die Zeitmessung stoppen. Die erreichte Zeit wird angezeigt.

Mit dem ersten Kippschalter kann ein Modus gewählt werden:

Im Einzelmodus muss dazu für jeden Stoß der grüne Startknopf gedrückt werden.
Dann beginnt eine Wartezeit (einstellbar über den Drehregler), bevor die Lampe leuchtet.

Im Automatikmodus läuft die Wartezeit, sobald die Ergebnisanzeige zurückgesetzt wird.
Das ist nach 5 Sekunden der Fall.

Durch erneutes Drücken der Starttaste kann die Wartezeit abgebrochen werden.

Mit dem zweiten Kippschalter kann das leitende Target zugeschaltet werden.
Ohne Target reicht es, die Spitze auszulösen.
Mit Target muss das Auslösen der Spitze auf dem Target erfolgen.


Debug-Modus:
Durch Drücken der Starttaste länger als 3 Sekunden wird in den Debug-Modus geschaltet.

Hier können Taster und Degen auf Funktion geprüft werden.

Ist "mit Target" ausgewählt, leuchtet die Lampe nur auf, wenn auch das Target getroffen wurde.
Es erscheint aber ein Text auf dem Display ("F Ok"), auch wenn das Target nicht getroffen wurde.