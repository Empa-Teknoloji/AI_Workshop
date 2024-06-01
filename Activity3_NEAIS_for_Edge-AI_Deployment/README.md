<p align="center">
    <img src="./Additionals/Empa-Workshops-Template-Banner2.png" alt="Accelerator Workshops" 
    style="display: block; margin: 0 auto"/>
</p>

# 3) NanoEdge AI Studio ile Edge-AI Çözümleri Geliştirme

Bu el kitabı, NanoEdge AI Studio kullanarak İnsan Aktivitesi Tanıma uygulamamızın geliştirme adımlarını uygulamanız için size rehberlik edecek.
<br>

### Kurulum

Öncelikle, aşağıdaki bağlantıdan kurulum adımlarını takip edeniz.

### ↳ [NanoEdge AI Studio Kurulum Kılavuzu](Kurulum.md)
### ↳ [STM32 İle Edge-AI Çözümleri: El Hareketi Sınıflandırma](Uygulama.md)
<br>

### Giriş

**NEAIS Nedir?**
- **Gömülü sistem mühendislerinin**, minimum AI bilgisi ile, gereksinimleri için ideal AI modelini zahmetsizce bulmalarını sağlar.
- İlk olarak **Cartesiam** (şimdi **ST**'ye ait) tarafından gömülü C yazılımı çalıştıran MCU'lar için geliştirilmiştir.

**Nasıl çalışır?**
- PC'de **yerel olarak** çalışır,
- Girdi verilerini içeri alır,
- Ön işlemeler, modeller ve parametrelerin binlerce kombinasyonunu keşfeder,
- Bir **kütüphane** (model, ön işlemeleri ve fonksiyonları) oluşturur,

**Neler yapamaz?**
- Herhangi bir girdi verisi **sağlamaz**. **Kullanıcıların nitelikli veriye sahip olması gerekir.**
- **Son projede** uygulanacak **hazır C kodu** sağlamaz.
- NEAIS esas olarak **sensör uygulamaları** için yapılmıştır.

**Özellikler**
- **ML uzmanlığı** gerektirmez.
- MCU hafızasını **verimli** kullanır.
- MCU'ların üzerinde çalışmak için optimize edilmiştir.

**Genel Adımlar**
- Proje ayarlarını yapılandırma
- Sinyalleri içe aktar
- **Benchmark** uygula
- Kütüphaneler (modeller) **bul ve karşılaştır**
- Kütüphaneleri **test et**
- **MCU'ya göm**

**Benchmark Adımları**
- Sinyal **ön işleme**
- ML modellerini **keşfetme**
- Optimum **hiperparametre** tarama

**Ön işleme süreçleri**
- **Data Logger (DL)**
- **Data Manipulation (DM)**
- **Sampling Finder (SF)**

**Modeller**
- **Anomaly Detection (AD)**
    - "Modellerin hedef ortama kendiliğinden uyum sağlamalarını ve anormallikleri kendiliğinden tespit etmelerini istiyorum."
- **1-Class Sınıflandırma (1CC)**
    - "Herhangi bir aykırı değeri tespit etmek istiyorum."
- **n-Class Sınıflandırma (nCC)**
    - "Hangi sorunların meydana geldiğini isim isim bilmek istiyorum."
- **Extrapolation (E)**
    - "Düzeltici eylemler için zaman kazanmak amacıyla titreşim seviyesini önceden tahmin etmek istiyorum."