/*==================
WareHouse Path Planner
WANG Yuchen

Copyright (c) <2017>, <WANG Yuchen>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the organization nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <WANG Yuchen> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

__________                                                   .__                 
\______   \_______  ____   ________________    _____   _____ |__| ____    ____   
 |     ___/\_  __ \/  _ \ / ___\_  __ \__  \  /     \ /     \|  |/    \  / ___\  
 |    |     |  | \(  <_> ) /_/  >  | \// __ \|  Y Y  \  Y Y  \  |   |  \/ /_/  > 
 |____|     |__|   \____/\___  /|__|  (____  /__|_|  /__|_|  /__|___|  /\___  /  
                        /_____/            \/      \/      \/        \//_____/   
.__                                                                              
|__| ______                                                                      
|  |/  ___/                                                                      
|  |\___ \                                                                       
|__/____  >                                                                      
        \/                                                                       
  _____           ._._.                                                          
_/ ____\_ __  ____| | |                                                          
\   __\  |  \/    \ | |                                                          
 |  | |  |  /   |  \|\|                                                          
 |__| |____/|___|  /___                                                          
                 \/\/\/                                                          

*/

#include <stdio.h>
#include <stdlib.h>

#define MAXORDERS 100
#define MAXITEMS 10
#define CORWIDTH 6.4 
#define BINWIDTH 3.8
#define MAXNUMOFORD 2 /*maximum number of orders per picker in stage 4*/

/* defining item location*/
typedef struct
{
    int it_num;
    int row;
    char col;

}itemloc_t;

/* defining an order for a customer*/
typedef struct
{
    int cust;
    int numitems;
    itemloc_t items[MAXITEMS];

}order_t;


/*initialising miscellaneous functions*/
void print_stage_title(int stage_num);

void print_customer_data(order_t data[], int tot_order);

void item_swap(itemloc_t *item1, itemloc_t *item2);

void sort_items(itemloc_t itemlist[], int num_items);

double calculate_distance(itemloc_t shoplist[], 
                      int item_count, double numcol, double numrow);

void fill_picker(itemloc_t picker[], itemloc_t order[], int init, int final);


/*initialising stage functions*/

void stage1(order_t data[], int tot_order, int tot_items);
void stage2(order_t data[], int tot_order);
void stage3(order_t data[], int tot_order, double numcol, double numrow);
void stage4(order_t data[], int tot_order, double numcol, double numrow);



/* main program binds it all together*/ 
int main(int argc, char *argv[])
{   
    /* temp storage for all the input data */
    int customer = 0, num_of_items = 0;
    int item_num = 0, row_num = 0;
    char column;

    /* storage for all of the output data */
    order_t orders[MAXORDERS];
    int total_items = 0, order_count = 0,i;

    /* main loop -- read the data, one customer at a time, and
    storing it in an array of order data */
    while(scanf("%d %d",&customer,&num_of_items) != EOF){
        orders[order_count].cust = customer;
        orders[order_count].numitems = num_of_items;
        for (i = 0; i < num_of_items; i++)
        {
            scanf("%d %d%c",&item_num,&row_num,&column);
            orders[order_count].items[i].it_num = item_num;
            orders[order_count].items[i].row = row_num;
            orders[order_count].items[i].col = column;
            total_items++;
        }
        order_count++;
    
    }
    /* first stage output */
    print_stage_title(1);
    stage1(orders,order_count,total_items);

    /* second stage output */
    print_stage_title(2);
    stage2(orders, order_count);

    
    /*transforming input #col and #row to doubles for stage 3&4*/
    double num_row = atof(argv[1]);
    double num_col = atof(argv[2]);

    /*third stage output*/
    print_stage_title(3);
    stage3(orders, order_count, num_col, num_row);

    /*fourth stage output*/
    print_stage_title(4);
    stage4(orders, order_count, num_col, num_row);

    return 0;
}


void print_stage_title(int stage_num){
    /* prints the stage number passed as an argument */
    printf("Stage %d\n-------\n", stage_num);
}

void print_customer_data(order_t data[], int cust_num){
    /* prints details for the specified customer */
    int i;
    printf("  customer %d, %2d items, bins: ",
                                  data[cust_num].cust, data[cust_num].numitems);
    
    for (i = 0; i < data[cust_num].numitems; i++)
    {
        printf("%2d%c ",data[cust_num].items[i].row, 
                                  data[cust_num].items[i].col);
    }
    printf("\n");
}

void sort_items(itemloc_t itemlist[], int num_items){
    
    /* Sort items in the itemlist passed as is specifed in the specifications  
       Code adapted from Moffat, A. (2012).  */
    
    int i,j;
    for (i = 1; i < num_items; i++)
    {
        for (j = i-1; j >= 0; j--)
        {    
             /* Sorting items according to colume num in ascending order */
             if (itemlist[j].col>itemlist[j+1].col)
             {
                item_swap(&itemlist[j], &itemlist[j+1]);

             }
             else if (itemlist[j+1].col==itemlist[j].col)
             {  
                /* column num even, sorting rows in descending order */
                if (itemlist[j].col%2==0 && itemlist[j].row < itemlist[j+1].row)
                {
                    item_swap(&itemlist[j], &itemlist[j+1]);
                }
                /* column num odd, sort rows in ascending order */
                else if(itemlist[j].col%2==1 && itemlist[j].row > 
                                                             itemlist[j+1].row)
                {
                    item_swap(&itemlist[j], &itemlist[j+1]);
                }

             }
        }
    }
}

void item_swap(itemloc_t *item1, itemloc_t *item2){
    /* Swaps the location of two items based on pointer passed
       Code adapted from int_swap in Moffat, A. (2012). */
    itemloc_t temp = *item1;
    *item1 = *item2;
    *item2 = temp;
}

double calculate_distance(itemloc_t shoplist[], int item_count, 
                                                 double numcol, double numrow){
    
    /*mandatory distance per order*/
    double entryexit = BINWIDTH;
    double horizontd = (numcol-1)*CORWIDTH;

    /* temp storage for distance specific to order*/
    double vertd_per_cor = (numrow+1)*BINWIDTH, verticald;
    
    char corridor = shoplist[0].col;
    int num_cor_travd, i;

    /* number of corridor/s travelled for first item */
    if (corridor%2)
    {
        num_cor_travd = 1;
    }
    else{
        num_cor_travd = 2;
    }
    
    /* counts all the corridors travelled for subsequent items*/
    for (i = 0; i < item_count; ++i)
    {
        if (shoplist[i].col != corridor)
        {
            if (shoplist[i].col%2 == corridor%2)
            {
                num_cor_travd += 2;
            }
            else{
                num_cor_travd += 1;
            }
            corridor = shoplist[i].col;
        }

    }
    
    /* if picker ends on the end of an odd corridor, 
       travel back through an even corridor to exit*/
    if (corridor%2)
    {
        num_cor_travd ++;
    }

    verticald = vertd_per_cor*num_cor_travd;
    
    /*adding all travel distances together and returning*/
    double total_distance = entryexit+horizontd+verticald;
    
    return total_distance;

}

void fill_picker(itemloc_t picker[], itemloc_t order[], int init, int final){
    /* fills the picker entered with items in the order entered as argument */
    
    int index = 0,i;
    for (i = init; i < final; i++)
    {
        picker[i] = order[index];
        index++;
    } 

}

/*Stage functions!!!*/

void stage1(order_t data[], int tot_order, int tot_items){
    
    /* generate the output format required for stage 1 */
    printf("  orders: %3d\n", tot_order);
    printf("  items : %3d\n", tot_items);
    
    print_customer_data(data, 0);
    if (tot_order > 1)
    {
        print_customer_data(data, tot_order-1);
    }
    printf("\n");
}

void stage2(order_t data[], int tot_order){
    
    int i;
    
    /* main loop sorting all orders */
    for (i = 0; i < tot_order; ++i)
    {
        sort_items(data[i].items, data[i].numitems);
    }
    
    /* generate the output format required for stage 2 */
    print_customer_data(data, 0);
    if (tot_order > 1)
    {
        print_customer_data(data, tot_order-1);
    }
    printf("\n");
}

void stage3(order_t data[], int tot_order, double numcol, double numrow){
    
    printf("  warehouse has %.lf rows and %.lf columns\n", numrow, numcol);
    double tot_distance = 0.0;
    int i;
    
    /* main loop calculating and summing distances for all orders */
    for (i = 0; i < tot_order; ++i)
    {
        double distance = calculate_distance(data[i].items, 
                                              data[i].numitems, numcol, numrow);
        tot_distance += distance;
        
        /* prints distance for first and last order*/
        if (i == 0)
        {
            printf("  customer %d, %2d items, pick distance: %.1lf metres\n", 
                                      data[i].cust, data[i].numitems, distance);
        }
        if (tot_order > 1 && i == tot_order-1)
        {
            printf("  customer %d, %2d items, pick distance: %.1lf metres\n", 
                                      data[i].cust, data[i].numitems, distance);
        }

    }
    
    /* calculating and printing average distance */
    double avg_distance = tot_distance/tot_order;
    printf("  average distance per order over %2d orders: %.1lf metres\n", 
                                                      tot_order, avg_distance);
    printf("\n");
}

void stage4(order_t data[], int tot_order, double numcol, double numrow){
    
    /* storage for output variables */
    double total_distance = 0.0;
    int count_pickers = 0, num_item_picker = 0, count_cust = 0, i;
    
    itemloc_t picker_order[MAXITEMS];
    
    /* main loop filling each picker and calculating distance 
       travelled by each individual picker */
    for (i = 0; i <= tot_order; i++)
    {
        if ((num_item_picker+data[i].numitems>MAXITEMS) || 
                              (count_cust >= MAXNUMOFORD) || i == tot_order)
        {   
            sort_items(picker_order, num_item_picker);
            total_distance += calculate_distance(picker_order, 
                                            num_item_picker, numcol, numrow);
            num_item_picker = 0;
            count_cust = 0;
            count_pickers++;

        }
        fill_picker(picker_order, data[i].items, num_item_picker,
                                             num_item_picker+data[i].numitems);
        num_item_picker+=data[i].numitems;
        count_cust++;
        
    }
    
    /* calculating and printing average distance travelled per order */
    double avg_distance_per_order = total_distance/tot_order;
    
    printf("  pickers required: %2d\n", count_pickers);
    printf("  average distance per order over %2d orders: %.1lf metres\n", 
                                            tot_order, avg_distance_per_order);
    printf("\n");
    
}