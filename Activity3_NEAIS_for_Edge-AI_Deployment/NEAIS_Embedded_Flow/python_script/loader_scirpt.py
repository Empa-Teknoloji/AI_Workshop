import tkinter as tk
from tkinter import filedialog
import os
import zipfile
def select_file():
    # Kullanıcının indirilenler klasörünü varsayılan olarak açar
    downloads_folder = os.path.join(os.path.expanduser("~"), "Downloads")
    file_path = filedialog.askopenfilename(initialdir=downloads_folder, filetypes=[("Zip files", "*.zip")])
    # Seçilen dosya yolunu ekranda gösterir ve içeriğini çıkartır
    if file_path:
        label.config(text=f"Seçilen dosya: {file_path}")
        extract_zip(file_path)
    else:
        label.config(text="Dosya seçilmedi.")
def extract_zip(file_path):
    # Scriptin çalıştığı dizinin bir üst dizinine çıkar
    script_directory = os.getcwd()
    target_base_directory = os.path.join(script_directory, "..", "NanoEdge_AI_Studio_Project", "Core", "Library")
    # Hedef dizinleri belirler
    inc_directory = os.path.join(target_base_directory, "Inc")
    src_directory = os.path.join(target_base_directory, "Src")
    os.makedirs(inc_directory, exist_ok=True)
    os.makedirs(src_directory, exist_ok=True)
    label.config(text=f"{file_path} çıkartılıyor...")
    root.update_idletasks()
    try:
        with zipfile.ZipFile(file_path, 'r') as zip_ref:
            # Zip dosyası içerisindeki belirli dosyaları hedef konumlara çıkar
            for file_name in zip_ref.namelist():
                if file_name.endswith("knowledge.h"):
                    zip_ref.extract(file_name, inc_directory)
                elif file_name.endswith("NanoEdgeAI.h"):
                    zip_ref.extract(file_name, inc_directory)
                elif file_name.endswith("libneai.a"):
                    zip_ref.extract(file_name, src_directory)
        # Dosya taşınma işlemi
        if os.path.exists(os.path.join(inc_directory, "knowledge.h")) and os.path.exists(os.path.join(inc_directory, "NanoEdgeAI.h")):
            label.config(text=f"knowledge.h ve NanoEdgeAI.h dosyaları {inc_directory} konumuna çıkarıldı.")
        if os.path.exists(os.path.join(src_directory, "libneai.a")):
            label.config(text=f"libneai.a dosyası {src_directory} konumuna çıkarıldı.")
    except zipfile.BadZipFile:
        label.config(text="Geçersiz zip dosyası.")
    except Exception as e:
        label.config(text=f"Bir hata oluştu: {e}")
# Ana pencereyi oluşturur
root = tk.Tk()
root.title("Dosya Seçici")
# Pencere boyutunu ayarlar
window_width = 400
window_height = 200
# Pencerenin ekranın ortasında olmasını sağlar
screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()
position_top = int(screen_height / 2 - window_height / 2)
position_right = int(screen_width / 2 - window_width / 2)
root.geometry(f"{window_width}x{window_height}+{position_right}+{position_top}")
# Dosya seçme butonunu oluşturur
button = tk.Button(root, text="Dosya Seç", command=select_file)
button.pack(pady=20)
# Seçilen dosya yolunu gösterecek etiketi oluşturur
label = tk.Label(root, text="Henüz dosya seçilmedi.")
label.pack(pady=20)
# Pencereyi çalıştırır
root.mainloop()