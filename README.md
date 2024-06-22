# UART FRAME CONTROL
## Projenin Amacı
Bu proje, bir UART çerçeve kontrol uygulamasının implementasyonunu içermektedir. 
Bu uygulamanın ana amacı, gelen veri paketlerini işlemek, bunların bütünlüğünü CRC (Döngüsel Artıklık Kontrolü) kullanarak doğrulamak ve UART iletişimi üzerinden doğru veri alımını sağlamaktır.
## Özellikler
-> Döngüsel Tampon: Gelen verileri verimli bir şekilde işlemek için döngüsel tampon(Circular Buffer) kullanır. <br/>
-> CRC Doğrulama: Gelen verilerin bütünlüğünü sağlamak için CRC hesaplama ve doğrulama yapar.<br/>
-> Paket Başlangıcı ve Sonu Kontrolü: Belirli başlık ve sonlandırıcı karakterlerle paket başlangıcını ve sonunu tespit eder.<br/>
-> UART DMA: UART iletişiminde gelen verileri DMA ile alır ve işler.<br/>

## Paket Yapısı
Uygulamada kullanılan veri paketleri belirli bir yapıdadır ve bu yapı, veri bütünlüğünü sağlamak için başlık, veri, CRC ve sonlandırıcı karakterleri içerir. 

### Paket Elemanları
-> Başlık (Header): Paket başlangıcını belirten özel karakterlerdir. Bu uygulamada, beş adet başlık karakteri kullanılır -> :A42* (HEADER1, HEADER2, HEADER3, HEADER4, HEADER5).<br/>
-> Veri Boyutu: İki baytlık bir dizi olup, gelen verinin boyutunu belirtir.<br/>
-> Veri: Taşınacak olan gerçek veri içeriğidir.<br/>
-> CRC: Verinin bütünlüğünü doğrulamak için kullanılan bir baytlık CRC değeri.<br/>
-> Sonlandırıcı (Trailer): Paket sonunu belirten özel karakterdir -> ; (TRAILER).
