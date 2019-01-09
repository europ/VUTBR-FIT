Dokumentácia pre 1. projekt IPK
===============================

## Popis
Tento projekt je napísaný v C++ jazyku a skladá sa z dvoch častí, klient *ftrest* a server *ftrestd*. Klient a server komunikujú medzi sebou pomocou HTTP cez RESTful API na transportnej vrstve TCP.

Na klientovy sa vytvorí na základe spúšťacích parametrov http hlavička prípadne sa pridá za hlavičku aj dáky obsah ktorý je odoslaný serveru.

Server spracuje hlavičku a následne realizuje požiadavku od klienta, server vie obslúžiť 20 klientov ktorý sú obslúžený podľa toho ako sú uložený do fronty. Po úspešnom vykonaní požiadavku odošle správu príslušnému klientovi.

Klient spracuje správu a následne sa podľa nej ukončí.

## Spustenie
Projekt treba preložiť pomocou Makefile.

## Klient **ftrest**
Klient sa spúšťa nasledovne: ./ftrest COMMAND REMOTE-PATH [LOCAL-PATH]
* COMMAND reprezentuje jeden príkaz z put,get,del,mkd,lst,rmd
* REMOTE-PATH je cesta vo formáte http://ADRESS/USER/PATH kde ADRESS=(ip:port alebo ip bez portu kde port je nastavený implicitne na 6677, ip je typu IPv4), USER=(meno), PATH=(cesta k súboru alebo priečinku)
* LOCAL-PATH je povinný parameter pri COMMAND=put a volitelný pri COMMAND=get, pri ostatných je zakázaný

### COMMAND
* del = vymaže soubor určený REMOTE-PATH na serveru
* get = skopíruje soubor z REMOTE-PATH do aktuálneho lokálneho priečinka alebo na miesto určené pomocou LOCAL-PATH ak je uvedené
* put = skopíruje soubor z LOCAL-PATH do priečinka REMOTE-PATH
* lst = vypíše obsah vzdialeného priečinka na standardní výstup
* mkd = vytvorí priečinok specifikovaný v REMOTE-PATH na serveru
* rmd = odstráni priečinok specifikovaný v REMOTE-PATH z serveru

### Príklad spustenia klienta
`````````````````````````````````````````````````````````````````````````
ftrest lst /tonda/dir
ftrest rmd /tonda/dir/emptydir
ftrest mkd /tonda/dir/newdir
ftrest get /tonda/dir/file
ftrest get /tonda/dir/filename.x /home/tonda/Desktop/newfilename.x
ftrest put /tonda/dir/newfilename.x filename.x
ftrest del /tonda/dir/filename.x
`````````````````````````````````````````````````````````````````````````

### Server **ftrestd**
Server sa spúšťa nasledovne: ./ftrestd [-r ROOT-DIR] [-p PORT]
* -r ROOT-DIR určí koreňový priečinok kam sa budú ukladať súbory pre užívateľov, default je current working directory
* -p PORT určí port, implicitná hodnota je 6677

### Príklad spustenia servera
```````````````````````````
ftrestd
ftrestd -p 9876
ftrestd -r /home
ftrestd -p 9876 -r /home
ftrestd -r /home -p 9876
```````````````````````````