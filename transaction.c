#include <stdio.h>
#include <stdlib.h>
#include "product.h"

struct product fill_prd(char str[]){
	struct product prd;
	prd.is_error = 0;
	char upc_str[100], quantity_str[100], name_str[100], price_str[100];
	int z = 0;
	int pos_str = 0;
	int pos_val = 0;

	// printf("%s\n", str);

	while(str[pos_str] != ' '){
		upc_str[pos_val] = str[pos_str];
		pos_str++;
		pos_val++;
	}
	upc_str[pos_val] = '\0';
	pos_str++;
	// printf("%s\n", upc_str);
	prd.upc = atoi(upc_str);

	pos_val = 0;
	while(str[pos_str] != ' '){
		prd.name[pos_val] = str[pos_str];
		pos_str++;
		pos_val++;
	}
	prd.name[pos_val] = '\0';
	pos_str++;
	// printf("%s\n", prd.name);

	pos_val = 0;
	while(str[pos_str] != ' ' && str[pos_str] != '\0'){
		price_str[pos_val] = str[pos_str];
		pos_str++;
		pos_val++;
	}
	price_str[pos_val] = '\0';
	pos_str++;
	// printf("%s\n", price_str);	
	prd.price = atoi(price_str);

	return prd;
}

struct product do_transaction(int upc){

	FILE *fptr;
	fptr = fopen("Database.txt", "r+");

	char str[100];
	struct product prd;
	prd.is_error = 1;

	int i, len;
	char ch;

	while(ch != EOF){

		i = 0;
		while(ch != '\n'){
			ch = fgetc(fptr);
			str[i] = ch;
			i++;
		}
		ch = fgetc(fptr);
		str[i] = '\0';

		prd = fill_prd(str);

		if(prd.upc == upc) return prd;

	}

	fclose(fptr);
	prd.is_error = 1;
	return prd;	

}