#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

typedef struct {
    int id;
    int mesafe;
    int hiz;
    int aci;
    const char* yon;
} Hedef;

const char* yonHesapla(int aci) {
    if (aci >= 315 || aci < 45) return "KUZEY";
    if (aci >= 45 && aci < 135) return "DOGU";
    if (aci >= 135 && aci < 225) return "GUNEY";
    if (aci >= 225 && aci < 315) return "BATI";
    return "BELIRSIZ";
}

static void kisaBekle(unsigned int mikroSaniye) {
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L
    
    struct timespec ts;
    ts.tv_sec = mikroSaniye / 1000000u;
    ts.tv_nsec = (long)(mikroSaniye % 1000000u) * 1000L;
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
        
    }
#else
    
    usleep(mikroSaniye);
#endif
}

int main() {
    int n;
    int menzil = 1000;
    srand((unsigned int)time(NULL));

    printf("================================================================================\n");
    printf("                            RADAR DENETIM VE ANALIZ SISTEMI                         \n");
    printf("================================================================================\n");
    printf("Sistem Kapasitesi: ");

    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Hata: Gecersiz giris.\n");
        return 1;
    }
    if (n <= 0) {
        fprintf(stderr, "Hata: Sistem kapasitesi pozitif olmali.\n");
        return 1;
    }

    
    Hedef* radarListesi = (Hedef*)malloc(sizeof(Hedef) * (size_t)n);
    if (!radarListesi) {
        fprintf(stderr, "Hata: Bellek ayrilamadi.\n");
        return 1;
    }

    int cokRiskliSayisi = 0;
    int riskliSayisi = 0;
    long toplamMesafe = 0;
    int enHizliHiz = 0;

  
    printf("\n%-5s %-10s %-10s %-12s %-12s %-20s\n",
           "ID", "SEKTOR", "MESAFE", "HIZ", "GRAFIK", "DURUM ANALIZI");
    printf("-------------------------------------------------------------------------------------\n");

    for(int i = 0; i < n; i++) {
        radarListesi[i].id = i + 1;
        radarListesi[i].mesafe = rand() % (menzil + 1);
        radarListesi[i].hiz = (rand() % 2400) + 100; // 100-2500 km/sa arası
        radarListesi[i].aci = rand() % 360;
        radarListesi[i].yon = yonHesapla(radarListesi[i].aci);

        
        toplamMesafe += radarListesi[i].mesafe;
        if(radarListesi[i].hiz > enHizliHiz) enHizliHiz = radarListesi[i].hiz;

       
        int bolen = menzil / 10;
        if (bolen <= 0) bolen = 1;
        int yildiz = radarListesi[i].mesafe / bolen;
        if (yildiz < 0) yildiz = 0;
        if (yildiz > 10) yildiz = 10;

        
        printf("#%-4d %-10s %6d m   %6d km/s ",
               radarListesi[i].id,
               radarListesi[i].yon,
               radarListesi[i].mesafe,
               radarListesi[i].hiz);

       
        for (int j = 0; j < yildiz; j++) putchar('*');
        for (int j = yildiz; j < 10; j++) putchar('.');
        printf("  ");

       
        if (radarListesi[i].mesafe <= 10) {
            printf("[!!] COK RISKLI");
            cokRiskliSayisi++;
        }
        else if (radarListesi[i].mesafe <= 150) {
            printf("[!] RISKLI");
            riskliSayisi++;
        }
        else {
            printf("Takipte");
        }
        printf("\n");

        kisaBekle(100000); // 100 ms

       
        if ((cokRiskliSayisi + riskliSayisi) > 3 && menzil == 1000) {
            menzil = 2000;
            printf("\n>>> SISTEM UYARISI: YOGUN TEHDIT! TARAMA ALANI %d METREYE GENISLETILDI <<<\n\n", menzil);
        }
    }

    
    int enYakin = radarListesi[0].mesafe, enUzak = radarListesi[0].mesafe;
    for(int i = 1; i < n; i++) {
        if(radarListesi[i].mesafe < enYakin) enYakin = radarListesi[i].mesafe;
        if(radarListesi[i].mesafe > enUzak) enUzak = radarListesi[i].mesafe;
    }

    printf("\n=========================================================================\n");
    printf("                         DETAYLI SISTEM ANALIZ RAPORU                      \n");
    printf("\n=========================================================================\n");
    printf("- Toplam Taranan Nesne      : %d\n", n);
    printf("- Ortalama Tespit Mesafesi  : %.2f m\n", (float)toplamMesafe / (float)n);
    printf("- Tespit Edilen En Yuksek Hiz: %d km/sa\n", enHizliHiz);
    printf("------------------------------------------------------------------------------------------\n");
    printf("- COK RISKLI HEDEF (0-10m)  : %d [DERHAL MUDAHALE]\n", cokRiskliSayisi);
    printf("- RISKLI HEDEF (11-150m)    : %d [ONLEM AKTIF]\n", riskliSayisi);
    printf("- GUVENLI BÖLGE HEDEFLERI   : %d\n", n - (cokRiskliSayisi + riskliSayisi));
    printf("------------------------------------------------------------------------------------------\n");
    printf("- TARANAN EN YAKIN NOKTA    : %d m\n", enYakin);
    printf("- TARANAN EN UZAK NOKTA     : %d m\n", enUzak);
    printf("- FINAL RADAR MENZILI       : %d m\n", menzil);


    free(radarListesi);
    return 0;
}

