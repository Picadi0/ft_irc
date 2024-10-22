# 42 Irc Project (WIP)

<!--toc:start-->
- [42 Irc Project (WIP)](#42-irc-project-wip)
  - [Usage](#usage)
  - [Description](#description)
  - [Development](#development)
    - [Dependencies](#dependencies)
    - [Installation](#installation)
    - [Terms](#terms)
<!--toc:end-->

---

## Usage

---
bu komutu calistirarak serveri baslatabilirsiniz.

```bash
 ./ircserv <port> <password>
```

## Description

---
Bu projenin amaci irc protocollerine uygun server yazmaktir.
Server port ve password alarak calisir.
size sunucuyu yonetmezniz icin basit bir terminal arayuzu ile sizi yaniz birakir.
### Terms

---

| Term | Description
| --- | ---
| IRC | Internet Relay Chat
| Socket | Iki bilgisayar arasinda veri alisverisi yapmak icin kullanilan bir yazilimdir.
| bind | Bir socketi bir adres ve port numarasi ile baglamak icin kullanilan bir fonksiyondur.
| listen | Socketin dinlemeye baslamasi icin kullanilan bir fonksiyondur.
| accept | Socketten gelen baglantilari kabul etmek icin kullanilan bir fonksiyondur.
| close | Socketi kapatmak icin kullanilan bir fonksiyondur.
| send | Sockete veri gondermek icin kullanilan bir fonksiyondur.
| recv | Socketten veri almak icin kullanilan bir fonksiyondur.
| poll | Bir veya birden fazla socketin durumunu kontrol etmek icin kullanilan bir fonksiyondur.
| AF_INET | Internet Protocol version 4 (IPv4)
| SOCK_STREAM | TCP
| SOCK_DGRAM | UDP
| IPPROTO_TCP | TCP protokolünü kullanmak için.
| IPPROTO_UDP | UDP protokolünü kullanmak için.

| int fcntl(int fd, int cmd, ...) | ... 
| --- | ---
| cmd | fcntl'in ne tür bir işlem yapacağını belirleyen komuttur. Yaygın komutlar şunlardır:
| F_GETFL | Dosya durum bayraklarını alır.
| F_SETFL | Dosya durum bayraklarını ayarlar (örneğin O_NONBLOCK gibi).
| F_GETFD | Dosya tanıtıcısının bayraklarını alır.
| F_SETFD | Dosya tanıtıcısının bayraklarını ayarlar.
| F_SETLK, F_SETLKW, F_GETLK | Dosya kilitleme işlemleri.
| O_NONBLOCK | Okuma/Yazma modunda işlem yarım kaldıysa/tamamlanamıyorsa threadi bekletme özelliğini kaldırır beklemez program devam eder

| Struct | Description
| --- | ---
| serverAddress | Sunucunun adresini tutan bir structtur.
| clientAddress | Clientin adresini tutan bir structtur.
| serverSocket | Sunucunun socketini tutan bir degiskendir.
| clientSocket | Clientin socketini tutan bir degiskendir.
| sin_family | Adresin ailesini tutan bir degiskendir.
| sin_port | Port numarasini tutan bir degiskendir.
| sin_addr | IP adresini tutan bir degiskendir.
