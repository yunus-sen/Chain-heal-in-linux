#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dllist.h"
#include "fields.h"

	//her bir oyuncuyu kendi listemde tutmak icin n isimli struct yapısını oluşturdum.
    struct n
        {
            int X, Y;
            int cur_PP, max_PP; //txt den okuduugm bilgileri tutacagım degişkenler
            char *name;

            struct n *next;//dugum yapımdan bir sonraki dugumu tutmak icin tanımladım.
            int visited;//dugumlerde dolaşırken ziyaret edip etmedigimi tutmak icin tanımladım.

            int gidebilecegi_dugum_sayisi;//dugum sayımı tuttum.
            struct n **gidebilecegi_dugum_list;//dugumlerimin adreslerini tutmak icin tanımladım.

            int healing;
        };
		typedef struct n Sdugum;//tip tanımı yaoarak struct n yazmadan kurtuldum.

	//gerekli degişkenler tanımladım.
    int initial_range, jump_range, num_jumps, initial_power = 0;
    double power_reduction;
    int en_iyi_iyilestirme;
    Sdugum **en_iyi_yol;//en iyi yolu buldugumda adreslerini tutmak icin double pointer kullandım.
    int *healing;
    Sdugum **temp_yol;//anlık yollarımı tutmak icin double pointer kullandım.
    Sdugum *kontrol;
    Sdugum *kontrol1;//en iyi yol nulmada kullandım.
	int *oyuncu;//en iyi yolda oyuncuları iyileştirme degerini tuttum.
	
	//fonksiyon protipini tanıttım.
    void bestPath(Sdugum* dugum, int hop, int num_jumpsL, int total_healing,int dugumSayisi);
	
    int main(int argc, char *argv[]){	
		//kullanıcıdan aldıgım degerlerin sayısını kontrol ettim
		if(argc!=6)
		{
			printf("hatali degerler girdiniz....");
			return 0;
		}
		
		//kod icinde gerekli adres tutucuları tanımladım.
		Sdugum *root;
		Sdugum *simdiki;
		Sdugum *newDugum;        
        root = NULL;
        int x, y, cur_PP, max_PP;//txt dosyasından okudugum degerleri atamak icin oluşturdum.
        char *name = malloc(sizeof(char) * 100);// txt den okudugum ismi atamak icin heapden yer ayırdım.
		
		//kullanıcıdan aldıgım parametreleri gerekli dönüşü yapıp degişkenlere atadım.
		initial_range=atoi(argv[1]);
		jump_range =atoi(argv[2]);
		num_jumps = atoi(argv[3]);
		initial_power =atoi(argv[4]);
		power_reduction=atof(argv[5]);

		//dosya okuma işlemlerini yaptım.her bir degişkeni okuyup dugum yapımda gerekli parametrelere atayıp listemi oluşturdum.
		IS is = new_inputstruct(argv[6]);
		int dugum_counter = 0;

		while(get_line(is) > 0) 
		{
			x =atoi(is->fields[0]);
			y =atoi(is->fields[1]);
			cur_PP =atoi(is->fields[2]);
			max_PP =atoi(is->fields[3]);
			name = is->fields[4];
			newDugum = (Sdugum*) malloc(sizeof(Sdugum));
			newDugum->X = x;
			newDugum->Y = y;
			newDugum->cur_PP = cur_PP;
			newDugum->max_PP = max_PP;
			newDugum->name = malloc(sizeof(char) * 100);
			strcpy(newDugum->name, name);

			if(dugum_counter <= 0)
			{
				root = newDugum;
				root->next = NULL;
				simdiki = root;
			}
			else
			{
				simdiki->next = newDugum;
				simdiki = newDugum;
				simdiki->next = NULL;
			}
			dugum_counter++;
  	    }  
		
		//yukarda tanımladıgım degişkenler icin heapden yer ayırdım.
		oyuncu=(int*)malloc(sizeof(int)*dugum_counter);
        simdiki = root;
        int looper = 0;
        Sdugum *dis_simdiki_dugum ;
        Sdugum *ic_simdiki_dugum;
        dis_simdiki_dugum = root;

        while(dis_simdiki_dugum)
        {
            int dugum_sayici = 0;
            ic_simdiki_dugum = root;
            while(ic_simdiki_dugum )
            {
                if( dis_simdiki_dugum != ic_simdiki_dugum)
				{
					if(sqrt(((dis_simdiki_dugum->X - ic_simdiki_dugum->X)*(dis_simdiki_dugum->X - ic_simdiki_dugum->X)) +
					((dis_simdiki_dugum->Y - ic_simdiki_dugum->Y)*(dis_simdiki_dugum->Y- ic_simdiki_dugum->Y))) <= jump_range)
					{
						dugum_sayici++;//gidebilecegim dugum sayılarını hesapladım.
					}
				}
                ic_simdiki_dugum = ic_simdiki_dugum->next;
            }
            dis_simdiki_dugum->gidebilecegi_dugum_sayisi = dugum_sayici;
            dis_simdiki_dugum = dis_simdiki_dugum->next;
        }
        dis_simdiki_dugum = root;

        while(dis_simdiki_dugum)
		{			
            dis_simdiki_dugum->gidebilecegi_dugum_list = (Sdugum**)malloc(sizeof(Sdugum*) * dis_simdiki_dugum->gidebilecegi_dugum_sayisi);
            int index_counter = 0;
            ic_simdiki_dugum = root;
			
            while(ic_simdiki_dugum)
			{
                if( dis_simdiki_dugum != ic_simdiki_dugum){
                    if(sqrt(((dis_simdiki_dugum->X - ic_simdiki_dugum->X)*(dis_simdiki_dugum->X - ic_simdiki_dugum->X)) + ((dis_simdiki_dugum->Y -
                    ic_simdiki_dugum->Y)*(dis_simdiki_dugum->Y - ic_simdiki_dugum->Y))) <= jump_range){
                        dis_simdiki_dugum->gidebilecegi_dugum_list[index_counter++]=ic_simdiki_dugum;
                    }
                }
                ic_simdiki_dugum = ic_simdiki_dugum->next;
            }
            dis_simdiki_dugum = dis_simdiki_dugum->next;
        }
		
		//fonksiyon icinde en iyi yolun ve anlık yolları tutmak icin adres tutucu diziler oluşturup heapde yer ayırdım.
        temp_yol = (Sdugum**)malloc(sizeof(Sdugum*) * num_jumps);
        en_iyi_yol = (Sdugum**)malloc(sizeof(Sdugum*) * num_jumps);
        healing =   (int*)malloc(sizeof(int)*num_jumps);
        kontrol = (Sdugum*) malloc(sizeof(Sdugum));
        kontrol1 = (Sdugum*) malloc(sizeof(Sdugum));
        en_iyi_iyilestirme = 0;
        simdiki = root;

		//pisegor degerine göre fonsiyonuma degerler gönderdim.
        for(int counter = 0; counter<dugum_counter; counter++)
		{
            if(sqrt(((root->X - simdiki->X)*(root->X - simdiki->X)) + ((root->Y - simdiki->Y)*(root->Y - simdiki->Y))) <= initial_range)
				{
                    bestPath(simdiki, 1, num_jumps,0,0);
                    simdiki = simdiki->next;

				}
		}
		
		printf("\n");
		//en iyi yolu bulup isimlerini ve iyileştirme degerlerini ekrana yazdırdım.
        for(int i=0; i<num_jumps;i++){
            printf("%s %d\n", en_iyi_yol[i]->name,healing[i]);
        }
		//toplam iyileşt,rmeyi ekrana yazdırdım.
        printf("Toplam_Iyilestirme:%d\n", en_iyi_iyilestirme);
		
		return 0;
}

void bestPath(Sdugum* dugum, int hop, int num_jumpsL, int total_healing,int dugumSayisi)
    {		
        int gereken_iyilestirme;
        int reset_init_power = initial_power;
		//dugumu ziyaret etmişsem ve atlama degerini gecmişsem return edip calıştırmayı engelledim.
        if(dugum->visited == 1 || hop>num_jumpsL)
        {
            dugum->visited = 0;
            return;
        }
		
        int count,bestIyilestirme;
		//dugumadres tutcu dizime elaman ekledim.
        temp_yol[dugumSayisi] = dugum;

		//listemi aşmamak icin kontrol eettirdim.
        if(dugumSayisi == num_jumpsL-1)
        {         
            for(int i = 0; i<num_jumpsL;i++)
            {
				//oyuncunun max degerini gecmemek icin gerekli enerjiyi hesapladım.
                gereken_iyilestirme = temp_yol[i]->max_PP - temp_yol[i]->cur_PP;
				//eger gerekli enerji iyileştirme enerjisinden küsükse diekt ekledim degil ise elimdeki tüm enerjiyle iyileştirdim.
                if(gereken_iyilestirme<initial_power)
				{
                    temp_yol[i]->healing = gereken_iyilestirme;
					bestIyilestirme=gereken_iyilestirme;
				}
                else
				{
                    temp_yol[i]->healing = initial_power;
					bestIyilestirme=initial_power;
				}
                initial_power = initial_power * (1-power_reduction);
				initial_power=rint(initial_power);
				total_healing = total_healing + temp_yol[i]->healing;
				oyuncu[i] = bestIyilestirme;
			}
			
			//yolları atadım.
			for( int dupC=0; dupC<num_jumpsL-1;dupC++)
			{
				for(int dupC2=dupC+1;dupC2<num_jumpsL;dupC2++)
					{
						kontrol = temp_yol[dupC];
						kontrol1 = temp_yol[dupC2];
						if(kontrol == kontrol1)
							total_healing = 0;
						}
			}  
			//tekrara kullanmak icin degerleri oluşturdum.
			initial_power = reset_init_power;
			if(total_healing>en_iyi_iyilestirme)
				{

					en_iyi_iyilestirme = total_healing;
					for(int x=0;x<num_jumpsL;x++)
					{
						en_iyi_yol[x]=temp_yol[x];
						healing[x]=oyuncu[x];
					}
				}
        }
		//tüm dügümleri gezerek  bestPath cagırdım. 
        for(count = 0; count<dugum->gidebilecegi_dugum_sayisi; count++)
        {
            dugum->visited = 1;
            bestPath(dugum->gidebilecegi_dugum_list[count], hop+1, num_jumpsL,total_healing,dugumSayisi+1);
        }
    }

