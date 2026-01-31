/*
//--------------------------------------------------------------------
        Как сортировать массив структуры
//--------------------------------------------------------------------
*/
int CompareDate(struct sensor* a,struct
sensor* b)
{
    if((a->year - b->year) != 0)
        return a->year - b->year;
    else if ((a->month - b->month) != 0)
        return a->month - b->month;
    else if ((a->day - b->day) != 0)
        return a->day - b->day;
    else if ((a->hour - b->hour) != 0)
        return a->hour - b->hour;
    else if ((a->t - b->t) != 0)
        return a->t - b->t;
    return 0;
}

//упорядочивающую его по дате
void SortByDate(struct sensor* info,int n)
{
    for(int i=0; i<n; ++i)
        for(int j=i; j<n; ++j)
            if(CompareDate(info+i,info+j)>0)
        //~ if(DateToInt(info+i)>=DateToInt(info+j))
                cgangeIJ(info,i,j);
}
//можно вызвать qsort
qsort(struct sensor* info,int n,sizeof(sensor),CompareDate);
//или привести к 64 битам
uint64_t DateToInt(struct sensor* info);

//--------------------------------------------------------------------
//        Как сортировать массив структуры qsort
//лучше использовать списки, чтобы перемещать только адреса, а не сами данные
//--------------------------------------------------------------------
int CompareDate(void* x,void* y)
{
    struct sensor* a = (struct sensor*)x;
    struct sensor* b = (struct sensor*)y;
    if((a->year - b->year) != 0)
        return a->year - b->year;
    else if ((a->month - b->month) != 0)
        return a->month - b->month;
    else if ((a->day - b->day) != 0)
        return a->day - b->day;
    else if ((a->hour - b->hour) != 0)
        return a->hour - b->hour;
    else if ((a->t - b->t) != 0)
        return a->t - b->t;
return 0;
}
//упорядочивающую его по дате
//можно вызвать qsort
qsort(struct sensor* info,int n,sizeof(sensor),CompareDate);