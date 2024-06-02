<p align="center">
    <img src="./Additionals/Empa-Workshops-Template-Banner2.png" alt="Accelerator Workshops" 
    style="display: block; margin: 0 auto"/>
</p>

# 3) NanoEdge AI Studio ile Edge-AI Çözümleri Geliştirme
Empa Electronics tarafından düzenlenen Accelerators Workshops serimizin Uçta Yapay Zeka adımına hoşgeldiniz.  
Bu kılavuz, NanoEdge AI Studio kullanılarak geliştilecek "El Karakterleri Tanıma" uygulamamızın geliştirme adımlarında size rehberlik edecektir.

## Kurulum
Öncelikle, aşağıdaki bağlantıyı kullanarak çalışma ortamı kurulum adımlarını takip ediniz.
### ↳ [NanoEdge AI Studio Kurulum Kılavuzu](Kurulum.md)
NanoEdge AI Studio programının kurulum adımlarını içerir.

## Uygulama
### ↳ [NEAIS İle Edge-AI Çözümleri: El Karakteri Sınıflandırma](Uygulama.md)
Aktivite içeriği olan "uçta yapay zeka" uygulamasının geliştirme adımlarını içerir.

## NanoEdge AI Studio

**1- NEAIS Nedir?**
- **Gömülü sistem mühendislerinin**, minimum AI bilgisi ile, gereksinimleri için ideal AI modelini zahmetsizce bulmalarını sağlar.
- İlk olarak **Cartesiam** (şimdi **ST**'ye ait) tarafından gömülü C yazılımı çalıştıran MCU'lar için geliştirilmiştir.

**2- Nasıl çalışır?**
- PC'de **yerel olarak** çalışır,
- Girdi verilerini içeri alır,
- Ön işlemeler, modeller ve parametrelerin binlerce kombinasyonunu keşfeder,
- Bir **kütüphane** (model, ön işlemeleri ve fonksiyonları) oluşturur,

**3- Neler yapamaz?**
- Herhangi bir girdi verisi **sağlamaz**. **Kullanıcıların nitelikli veriye sahip olması gerekir.**
- **Son projede** uygulanacak **hazır C kodu** sağlamaz.
- NEAIS esas olarak **sensör uygulamaları** için yapılmıştır.

**4- Özellikler**
- **ML uzmanlığı** gerektirmez.
- MCU hafızasını **verimli** kullanır.
- MCU'ların üzerinde çalışmak için optimize edilmiştir.

**5- Genel Adımlar**
- Proje ayarlarını yapılandırma
- Sinyalleri içe aktar
- **Benchmark** uygula
- Kütüphaneler (modeller) **bul ve karşılaştır**
- Kütüphaneleri **test et**
- **MCU'ya göm**

**6- Benchmark Adımları**
- Sinyal **ön işleme**
- ML modellerini **keşfetme**
- Optimum **hiperparametre** tarama

**7- Ön işleme süreçleri**
- **Data Logger (DL)**
- **Data Manipulation (DM)**
- **Sampling Finder (SF)**

**8- Modeller**
- **Anomaly Detection (AD)**
    - "Modellerin hedef ortama kendiliğinden uyum sağlamalarını ve anormallikleri kendiliğinden tespit etmelerini istiyorum."
- **1-Class Sınıflandırma (1CC)**
    - "Herhangi bir aykırı değeri tespit etmek istiyorum."
- **n-Class Sınıflandırma (nCC)**
    - "Hangi sorunların meydana geldiğini isim isim bilmek istiyorum."
- **Extrapolation (E)**
    - "Düzeltici eylemler için zaman kazanmak amacıyla titreşim seviyesini önceden tahmin etmek istiyorum."

**Kaynaklar & Okuma Önerileri** 

1- [Wiki by STMicroelectronics - NanoEdge AI Studio](https://wiki.stmicroelectronics.cn/stm32mcu/wiki/AI:NanoEdge_AI_Studio)