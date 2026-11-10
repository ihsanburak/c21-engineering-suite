# Hızlı başlangıç — Türkçe

**C21 Engineering Suite 1.0 Classic Edition**, uygulama derlemesi **1.0.2**.

Hedef: özgün siyah-beyaz TI-Nspire Touchpad, non-CAS, OS 3.9.0.463 ve önceden çalışan uyumlu Ndless. [Kurulum kılavuzunu](INSTALL.md) inceleyin. Bu uygulama için işletim sistemini güncellemeyin, cihazı sıfırlamayın veya Ndless'i yeniden kurmayın.

## Kullanım

- Sayı tuşları numaralı seçeneği açar; **0, 10. seçenektir**. Ok tuşlarıyla ilerleyin. Enter veya Touchpad merkez tıklaması seçimi onaylar; Esc geri döner.
- **C:** hesapla. **F:** desteklenen araçlarda Formül/Liste modu. **O:** desteklenen çıkış değişkenini seç.
- Formül modunda sol/sağ/Tab değişkeni seçer, Enter düzenlemeyi açar; **U** giriş birimini, **Shift-U** çıkış birimini seçer.
- Liste modunda yukarı/aşağı alanı veya eylemi seçer. Enter düzenler/onaylar; giriş üzerindeyken sol/sağ birim seçimini açar.
- **H:** ilgili mühendislik el kitabını açar. Geri dönünce girilen değerler korunur.
- Ondalık sayı, geçerli negatif değer ve bilimsel E gösterimi kullanılabilir. EE/e^x, E girer. Del siler; Ctrl+Del temizler; Ctrl+sol/sağ başa/sona gider; Shift+Del imlecin sağını siler. Enter kaydeder, Esc iptal eder. Değeri değiştirdikten sonra yeniden hesaplayın.
- **S → Dil:** **1. English**, **2. Turkce ASCII**. Yeni kurulumda İngilizce kullanılır. Eski Unicode Türkçe tercihi güvenli biçimde Turkce ASCII olarak yüklenir. Değerler, birimler, mod ve geçerli sonuç korunur. Uygulama metinleri ASCII gösterilir; bu belge normal Türkçe yazımla hazırlanmıştır.

## Örnekler

**Ohm yasası:** Temel Elektronik altından akım hesabını seçin. V=12 V, R=100 Ω → **0.12 A**. Direnci 200 Ω yapıp yeniden hesaplayın → **0.06 A**.

**LC rezonans tasarımı:** RLC/rezonans altından gerekli kapasitans hesabını açın. f=868 MHz, L=10 nH, çıkış pF → yaklaşık **3.362024 pF**. L=20 nH için yaklaşık **1.681012 pF**.

**RF güç dönüşümü:** 100 mW → **20 dBm**; 200 mW → yaklaşık **23.0103 dBm**.

**LoRa paket süresi:** SF7, BW125 kHz, CR indeksi1, fiziksel yük20 bayt, IH0 (açık başlık), CRC1, önsöz8, DE0 → **56.576 ms**. SF12 için DE1 gereklidir; sonuç **1.318912 s** olur. Parametreleri gerçek modem ayarlarından alın.

Birimleri açıkça seçin. Hesaplama motoru SI kullanır. H ile formül, değişkenler, ölçüm cihazları, laboratuvar kullanımı, varsayımlar ve sık yapılan hataları inceleyin. El kitabındaki örnekler canlı hesap sonucunun yerine kullanılmaz.

## Sınırlar ve cihaz doğrulaması

LoRa modeli eski SX127x, SF7–12, BW125/250/500 kHz içindir. Mikroşerit modeli sıfır iletken kalınlığı varsayar. IPC akım tahmini güvenlik derecelendirmesi değildir. İstatistik araçları dört ortak-birimli ölçüm kullanır; SMD çözümleme yalnız sayısal kodları destekler. [Model sınırlarını](KNOWN_LIMITATIONS.md) okuyun.

Sahip, gerçek cihazda açılışı ve LC rezonans/Ohm yasası/mW-dBm örneklerini doğrulamıştır. V1.0.2 merkez tıklaması ve dil menüsünün son onayı beklenmektedir. Her hesap gerçek cihazda ayrı ayrı doğrulanmış değildir.
