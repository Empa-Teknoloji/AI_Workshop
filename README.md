<p align="center">
    <img src="Additionals/Empa-Workshops-Template-Banner2.png" alt="Accelerator Workshops" 
    style="display: block; margin: 0 auto"/>
</p>


## Accelerator Workshops'a Hoşgeldiniz!

Merhaba, 

Empa Electronics tarafından düzenlenen Accelerators Workshops serimizin Uçta Yapay Zeka adımına hoşgeldiniz. Bu açık-kaynak repository, workshop etkiliğimizde kullanabileceğiniz tüm gereksinimleri edinebilmeniz ve aktivitelere kolaylıkla eşlik edebilmeniz için sizinle paylaşılmıştır.

> _Not: Workshop aktiviteleri için sağlanan çalışma ortamlarının son hallerini edinmek için Güncellemeler başlığını kontrol ediniz._

## Workshop Seansları & Aktiviteler

Workshop etkinliğimizde gün boyunca sizi 4 farklı seans ile konuk ediyor olacağız.

| Aktivite | Seans | Zaman Aralığı | Gereksinimler |
| ------ | ------ | ------ | ------ | 
| Empa Electronics, Yapay Zeka Teorisi, Uçtan-Uca Geliştirme | Empa Electronics & Yapay Zeka | 09:00 - 10:30 | - |
| Vitis-AI İle AMD Platformlarında AI Çözümleri Geliştirme | AMD Platformlarında AI Çözümleri | 10:45 - 12:00 | Aktivite-1 Çalışma Ortamı |
| STM32 İle Edge-AI Çözümleri: El Hareketi Sınıflandırma | ST Platformları Üzerinde Edge-AI Çözümleri | 13:00 - 14:30 | Aktivite-2 Çalışma Ortamı |
| NEAIS ile El Karakteri Sınıflandırma | NanoEdge AI Studio İle Edge-AI Çözümleri  | 15:00 - 16:00 | Aktivite-3 Çalışma Ortamı |

## Aktivite Kaynakları
Uçta Yapay Zeka workshop etkinliğimizde gerçekleştirilecek aktivitelerde kullanılacak çalışma ortamları ve gerekli kurulumlar için sırasıyla her bir aktivite için oluşturulan kurulum kılavuzlarına giderek gerekli adımları uygulayınız. 
### ↳ [1) Vitis-AI ile FPGA Platformlarında AI Uygulamaları Geliştirme](Activity1_AI_Solution_with_Vitis-AI)
Yapay zeka çözümlerinin FPGA tabanlı AMD platformlarında kullanımının uygulamalı anlatımını içerir.
### ↳ [2) MCU Platfromları İçin Edge-AI Çözümleri Geliştirme](Activity2_Bare-Metal_Edge-AI_Solution)
Çoğunluğu Tiny-ML konseptine uygun "uçta yapay zeka" uygulamalarının ST Microelectronics ürünleri üzerinde çözümleştirilmesine yönelik uygulamalı anlatımı içerir.
### ↳ [3) NanoEdge AI Studio ile Edge-AI Çözümleri Geliştirme](Activity3_NEAIS_for_Edge-AI_Deployment)
Tiny-ML uygulamaları geliştirmede güçlü bir alternatif oluşturan NanoEdge AI Studio ile uçta yapay zeka uygulamaları geliştirme adımlarını içerir. 

## Dizin Yapısı

Repository içerisindeki her bir klasör, etkinliğimizde yer alacak bir yapay zeka çözümü uygulamasına ait çalışma ortamlarını ve gerekli kurulumları içermektedir.

```
Workshop Repository
├── Activity1_AI_Solution_with_Vitis-AI
│   ├── Kaynak Kod & Materyaller
│   └── README.md (Aktivite-1 Kılavuzu)
├── Activity2_Bare-Metal_Edge-AI_Solution
│   ├── Kaynak Kod & Materyaller
│   └── README.md (Aktivite-2 Kılavuzu)  
└── Activity3_NEAIS_for_Edge-AI_Deployment
    ├── Kaynak Kod & Materyaller
    └── README.md (Aktivite-3 Kılavuzu) 
```

## Ön Gereksinimler - Checklist
Workshop etkiliğinde kullanılacak çalışma ortamlarının kurulumları sonrası hazırladığımız checklist ile gereksinimlerin kontrolünü sağlayabilirsiniz.

**Aktivite-1**
- [ ] Python3.8
- [ ] Docker
- [ ] Vitis-AI Docker Container Ortamı
- [ ] Aktivite-1 Kaynak Dosyaları

**Aktivite-2**
- Alternatif-1: Bulut Python Ortamı 
    - [ ] Colab Notebook Örneği (Kurulum Gerektirmez, Sadece Geliştirme)
- Alternatif-2: Yerel (Bilgisayarınızda) Python Ortamı 
    - [ ] Python3.10
    - [ ] VSCode Editor
    - [ ] Git
    - [ ] STM32CubeMX & STM32CubeAI
- [ ] Aktivite-2 Kaynak Dosyaları

**Aktivite-3**
- [ ] NanoEdge AI Studio
- [ ] Aktivite-3 Kaynak Dosyaları

## Güncellemeler
Workshop etkinliğimizde gerekli çalışma ortamları üzerindeki güncellemeleri bu başlık altında takip edebilirsiniz.
```
Versiyon-1: 3 Haziran 2024  
Tüm aktiviteler için temel bileşenleri içeren kılavuzlar repository içerisinde paylaşıldı.
```
