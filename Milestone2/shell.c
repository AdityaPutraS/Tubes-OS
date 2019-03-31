#define TRUE 1
#define FALSE 0
#define EMPTY 0x00

#define echo 1
#define cd 2
#define ls 3
#define mkdir 4
#define rm 5
#define cat 6
#define runLocal 7
#define runGlobal 8

void pS(char *string, int newLine);
void pI(int i, int newLine);
void pC(char c, int newLine);
void len(char *string, int *length);
void count(char *string, char c, int *banyak);
char isSame(char *s1, char *s2);
void copy(char *string, char *copied, int start, int length);
int find(char *s, char c);
int getCommandType(char *string);
void concat(char *s1, char *s2, char isiNull, int len1);
void replace(char *s, char cari, char tukar);
void split(char *string, char separator, char splitted[64][128]);
void clear(char *buffer, int length);

int main()
{
    int b1, b2;
    char isExit;
    char curDir;
    char inputSeparator[1];
    char input[100], temp[100];
    int inputLen, splitLen;
    int type, i, copyStart, copyEnd;
    char argc, succ;
    char argv[64][128];
    isExit = FALSE;
    while (!isExit)
    {
        //Get input user
        interrupt(0x21, (0 << 8) | 0x0, "$ ", 0, 0);
        interrupt(0x21, (0 << 8) | 0x1, &input, 0, 0);
        len(input, &inputLen);
        count(input, 0x20, &splitLen);
        //Get curdir
        interrupt(0x21, 0x21, &curDir, 0, 0);
        pS("curDir : ", FALSE);
        if (curDir == 0x00)
        {
            pS("di 0", TRUE);
        }
        else if (curDir == 0xFF)
        {
            pS("di Root", TRUE);
        }
        else
        {
            pI(curDir, TRUE);
        }
        //Get type commandnya
        type = getCommandType(input);
        //Get argc dan argv nya
        argc = splitLen;
        split(input, 0x20, argv);
        if (type == echo)
        {
            pS("Command echo", TRUE);
            interrupt(0x21, 0x20, curDir, argc, argv + 1);
            interrupt(0x21, curDir << 8 | 0x6, "echoUtil", 0x2000, &succ);
        }
        else if (type == cd)
        {
            pS("Command cd", TRUE);
        }
        else if (type == ls)
        {
            pS("Command ls", TRUE);
            if (argc != 0)
            {
                pS("ls tidak menerima parameter apapun", TRUE);
            }
            else
            {
                interrupt(0x21, 0x20, curDir, argc, argv + 1);
                // pS("argv : ", FALSE);
                // pS(argv[0],TRUE);
                interrupt(0x21, curDir << 8 | 0x6, "ls", 0x2000, &succ);
            }
        }
        else if (type == mkdir)
        {
            //TODO : mkdir bisa banyak parameter
            pS("Command mkdir", TRUE);
            if (argc != 1)
            {
                pS("Jumlah parameter mkdir harus 1", TRUE);
            }
            else
            {
                interrupt(0x21, 0x20, curDir, argc, argv + 1);
                // pS("argv : ", FALSE);
                // pS(argv[0],TRUE);
                interrupt(0x21, curDir << 8 | 0x6, "mkdir", 0x2000, &succ);
            }
        }
        else if (type == rm)
        {
            pS("Command rm", TRUE);
            if (argc != 1)
            {
                pS("Jumlah parameter rm harus 1", TRUE);
            }
            else
            {
                //KODE RM DI SHELL AGAR TIDAK BISA DIHAPUS MENGGUNAKKAN RM
                interrupt(0x21, curDir << 8 | 0x0A, argv[1], &b1, 0);
                interrupt(0x21, curDir << 8 | 0x09, argv[1], &b2, 0);
                if (b1 == -1 && b2 == -1)
                {
                    pS("PATH NOT_FOUND", TRUE);
                }
                else if (b1 == 0 || b2 == 0)
                {
                    pS("SUCCESS", TRUE);
                }
                else
                {
                    pS("FAILED, UNKNOWN REASON", TRUE);
                }
            }
        }
        else if (type == cat)
        {
            pS("Command cat", TRUE);
        }
        else if (type == runLocal)
        {
            pS("Command runLocal", TRUE);
        }
        else if (type == runGlobal)
        {
            pS("Command runGlobal", TRUE);
        }
    }
}

void clear(char *buffer, int length)
{
    int i;
    for (i = 0; i < length; ++i)
    {
        buffer[i] = EMPTY;
    }
}

void split(char *string, char separator, char splitted[64][128])
{
    int i, j, k;
    i = 0;
    j = 0;
    k = 0;
    clear(splitted[0], 128);
    while (string[i] != '\0')
    {
        if (string[i] == separator)
        {
            splitted[j][k] = '\0';
            j += 1;
            k = 0;
            clear(splitted[j], 128);
        }
        else
        {
            splitted[j][k] = string[i];
            k += 1;
        }
        i += 1;
    }
    splitted[j][k] = '\0';
}

void replace(char *s, char cari, char tukar)
{
    int length, i;
    len(s, &length);
    for (i = 0; i < length; i++)
    {
        if (s[i] == cari)
        {
            s[i] = tukar;
        }
    }
    s[length] = '\0';
}

void concat(char *s1, char *s2, char isiNull, int len1)
{
    int i;
    i = 0;
    while (s2[i] != '\0')
    {
        s1[len1 + i + isiNull] = s2[i];
        i += 1;
    }
    s1[len1 + i + isiNull] = '\0';
}

int getCommandType(char *s)
{
    char command[20];
    int idxCom, lenS;
    char sama;
    int idxSpasi = find(s, 0x20);
    len(s, &lenS);
    if (idxSpasi == -1)
    {
        idxSpasi = lenS;
    }
    copy(s, command, 0, idxSpasi);
    // pS("Command : ", FALSE);
    // pS(command, TRUE);
    if (isSame(command, "echo"))
    {
        return echo;
    }
    if (isSame(command, "cd"))
    {
        return cd;
    }
    if (isSame(command, "ls"))
    {
        return ls;
    }
    if (isSame(command, "mkdir"))
    {
        return mkdir;
    }
    if (isSame(command, "rm"))
    {
        return rm;
    }
    if (isSame(command, "cat"))
    {
        return cat;
    }
    //Beda semua
    if (command[0] == '.' && command[1] == '/')
    {
        return runLocal;
    }
    else
    {
        return runGlobal;
    }
}

int find(char *s, char c)
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i] == c)
        {
            return i;
        }
        i++;
    }
    return -1;
}

void pS(char *string, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x0, string, newLine, 0);
}

void pI(int i, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x24, i, newLine, 0);
}

void pC(char c, int newLine)
{
    interrupt(0x21, (0 << 8) | 0x25, c, newLine, 0);
}

void len(char *string, int *length)
{
    *length = 0;
    while (string[*length] != '\0')
    {
        *length += 1;
    }
}

void count(char *string, char c, int *banyak)
{
    char temp[2];
    int i = 0;
    *banyak = 0;
    while (string[i] != '\0')
    {
        if (string[i] == c)
        {
            *banyak += 1;
        }
        i += 1;
    }
}

char isSame(char *s1, char *s2)
{
    int len1, len2, i;
    char tempResult;
    len(s1, &len1);
    len(s2, &len2);
    if (len1 != len2)
    {
        return FALSE;
    }
    else
    {
        i = 0;
        tempResult = TRUE;
        while (s1[i] != '\0' && tempResult)
        {
            if (s1[i] != s2[i])
            {
                tempResult = FALSE;
            }
            i += 1;
        }
    }
    return tempResult;
}

void copy(char *string, char *copied, int start, int length)
{
    int lenS, i;
    len(string, &lenS);
    copied[0] = '\0';
    //Validasi start
    if (start < lenS)
    {
        if (length > 0)
        {
            i = start;
            while (length > 0 && string[i] != '\0')
            {
                copied[i - start] = string[i];
                i += 1;
                length -= 1;
            }
            copied[i - start] = '\0';
        }
    }
}