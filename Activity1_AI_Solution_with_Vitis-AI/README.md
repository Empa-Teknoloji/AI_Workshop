<p align="center">
    <img src="./Additionals/Empa-Workshops-Template-Banner2.png" alt="Accelerator Workshops" 
    style="display: block; margin: 0 auto"/>
</p>

# 1) Vitis-AI ile FPGA Platformlarında AI Uygulamaları Geliştirme

Empa Electronics tarafından düzenlenen Accelerators Workshops serimizin Uçta Yapay Zeka adımına hoşgeldiniz.  
Bu kılavuz, açık kaynaklı makine öğrenimi kütüphaneleri ve literatür makine öğrenimi modelleri kullanılarak elde edilen bir derin öğrenme modelinin AMD ürünü FPGA platformları çözüm haline getirilme adımlarının uçtan uca incelendiği "YOLOv5 ile Kria Platformu Üzerinde Yaygın Nesnelerin Tespiti" uygulamamızın geliştirme adımlarında size rehberlik edecektir.

Aktivite içeriği olan "YOLOv5 ile Kria Platformu Üzerinde Yaygın Nesnelerin Tespiti” uygulaması, AMD tarafından Vitis-AI ortamı kullanılarak FPGA tabanlı platformlarda açık kaynaklı bir nesne algılama AI modelinin uçtan uca uygulanmasını içeren, ham bir depo kaynağından başlayarak uçta konuşlandırılmış bir çözüme kadar kapsamlı bir demo çalışmasıdır. Model, COCO veri setinden insan, araba, köpek, cep telefonu, şişe, dizüstü bilgisayar gibi 80 yaygın sınıfı 2 boyutlu sınırlayıcı kutular olarak tespit edebilmektedir. Aktivite içeriğindeki demo çalışması, model mimarisi incelemesi, mimari modifikasyonları, niceleme ve daha fazlası dahil olmak üzere uyumluluk için gerekli tüm adımları sunmayı amaçlamaktadır.

Bu çalışma için resmi Ultralytics deposundaki YOLOv5s (small) model örneği kullanılmıştır. DPU uyumluluğu için iki farklı modifikasyon gereklidir ve uygulanmıştır: Model kafasının ileri geçiş adımı, bazı işlem sonrası işlemleri hariç tutmak için değiştirilmiştir. Ayrıca, SiLU aktivasyonu Vitis-AI tarafından desteklenmediği için LeakyReLU aktivasyonu ile değiştirilmiştir. Değiştirilmiş model mimarisi, yeni bir model örneği elde etmek adına az miktarda eğitim verisi kullanılarak yeniden eğitilmiştir. Böylelikle, YOLOv5 model örneği tek bir DPU alt grafiği olarak elde edilmiştir.
Değiştirilmiş YOLOv5 model örneği, DPU IP’sinin floating-point aritmetik kısıtlamaları nedeniyle modelin aritmetik olarak uyumlu bir versiyonunu oluşturmak için INT8'e niceleme (quantization) işlemine tabi olur. Diğer adımlarda, model mimarisi belirli bir DPU mimarisi kullanılarak derlenir ve bir Kria KV260 Vision AI Starter Kit üzerinde Vitis-AI Runtime GraphRunner API kullanılarak işletilebilir.

## Kurulum
Öncelikle, aşağıdaki bağlantıyı kullanarak çalışma ortamı kurulum adımlarını takip ediniz.
### ↳ [Çalışma Ortamı Kurulumu](Kurulum.md)
Aktivite için gerekli program ve gereçlerin kurulum adımlarını içerir.

## Uygulama
### ↳ [Vitis-AI İle FPGA Platformlarında AI Uygulamaları Geliştirme](./yolov5/Uygulama_Quantize_Compile_YoloV5s_using_Vitis-AI.ipynb)
Aktivite içeriği olan "YOLOv5 ile Kria Platformu Üzerinde Yaygın Nesnelerin Tespiti" uygulamasının Vitis-AI çözümü tarafından sağlanan resmi Docker container ortamı üzerinde geliştirme adımlarını içerir.

### ↳ [Model Implementasyonu: YOLOv5 ile Kria Platformu Üzerinde Yaygın Nesnelerin Tespiti](Implementasyon.md)
Aktivite içeriği olan "YOLOv5 ile Kria Platformu Üzerinde Yaygın Nesnelerin Tespiti" uygulamasının AMD Kria KV260 Starter Kit platformu üzerinde test ve çözümleştirme adımlarını içerir.