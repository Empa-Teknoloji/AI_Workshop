<p align="center">
    <img src="./Additionals/Empa-Workshops-Template-Banner2.png" alt="Accelerator Workshops" 
    style="display: block; margin: 0 auto"/>
</p>

# 1) Vitis-AI ile FPGA Platformlarında AI Uygulamaları Geliştirme

Aktivite içeriği ile oluşturulan uygulamanın hedef platform üzerinde dağıtımı için listelenen adımları sıralı şekilde uygulayınız.

## Uygulamanın Implementasyonu

**1- Hedef Platform Kurulumları**
Hedef platformun gerekli kaynak dosyaları ile beraber hazır halde edinimi için resmi AMD Vitis-AI dokümantasyonu üzerindeki Target Installation sayfasında verilen adımları uygulayınız.

Kaynak: [https://xilinx.github.io/Vitis-AI/3.0/html/docs/quickstart/mpsoc.html#setup-the-target](https://xilinx.github.io/Vitis-AI/3.0/html/docs/quickstart/mpsoc.html#setup-the-target)

**2- Uygulama Kaynak Kodlarının Hedef Platforma Taşınması**
Aktivite içeriğinde verilen Scripts klasörünün ve derlenmiş modelin (_model_compiled_) hedef platform üzerinde yer bir dosya konumuna taşınması ve Python dili kullanımıyla çalıştırılması gerekmektedir. 

SSH bağlantısı için gerekli lokal IP adresini (_inet: 192.168..._) hedef platform üzerindeki terminalde "ifconfig" komutunu çalıştırarak edininiz. 
```
(target) ifconfig
```

Daha sonra bağlantı için verilen komutu kullanarak SSH bağlantısına sahip bir terminal oluşturunuz (xxx ile verilen yerlere sizin bağlantınız için uygun değeri yazınız).
```
(host) ssh root@192.168.xxx.xxx
```

SSH bağlantısı oluşturulan terminal yardımı ile hedef platform üzerinde uygulama dosyalarının saklanması için gerekli bir klasör oluşturunuz.
```
(target) mkdir Vitis-AI && cd Vitis-AI
```
Host terminal üzerinde aktivite klasörü (_Activity1_AI_Solution_with_Vitis-AI_) içerisine gidiniz ve Scripts klasörü içerisindeki kaynak dosyalarını "scp" komutu yardımıyla hedef platform üzerine kopyalayınız.
```
(host) scp -r Scripts root@192.168.xxx.xxx:/home/root/Vitis-AI/
```

**3- Uygulamanın Hedef Platform Üzerinde Çalıştırılması**
Uygulamanın Kria KV260 platformunda Petalinux işletim sistemi üzerinde çalıştırılması için ön-yüklü olarak gelen Python ve içerisindeki Vitis-AI Runtime kütüphanesi kullanılmaktadır.

Uygulamanın çalıştırılması için Scripts klasörü içerisindesi "app_" ön eki ile başlayan kaynak kodları içerisindeki "path_" ile başlayan değişkenlerin model dosyalarının konumunu gösterecek şekilde değiştirilmesinin ardından Python yorumlayıcı kullanılarak çalıştırılmaktadır.
```
python3 app_yolov5_video.py
```