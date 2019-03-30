#define TRUE 1
#define FALSE 0

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
void len(char *string, int *length);
void count(char *string, char c, int *banyak);
char isSame(char *s1, char *s2);
void copy(char *string, char *copied, int start, int length);
int find(char *s, char c);
int getCommandType(char *string);

int main()
{
    char isExit;
    char curDir;
    char inputSeparator[1];
    char input[100], temp[100];
    int inputLen, splitLen;
    int type, i, copyStart, copyEnd;
    char argc, succ;
    char *argv[64];
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
        pS("curdir : ", FALSE);
        pI(curDir,TRUE);
        //Get type commandnya
        type = getCommandType(input);
        if (type == echo)
        {
            argc = splitLen;
            //pI(argc, TRUE);
            copyStart = find(input, 0x20);
            for(i = 0; i < argc; i++)
            {
                copyEnd = find(input+copyStart+1, 0x20);
                if(copyEnd == -1)
                {
                    copyEnd = inputLen -copyStart-1;
                }
                pS("letak argv : ", FALSE);
                pI(argv[0][0], TRUE);
                copy(input, argv[i], copyStart, copyEnd+1);
                pS("letak argv : ", FALSE);
                pI(argv[0][0], TRUE);
                // pI(copyStart, FALSE);
                // pI(copyEnd, FALSE);
                // pS(argv[i], TRUE);
                copyStart += copyEnd+1;
            }
            interrupt(0x21, 0x20, curDir, argc, argv);
            //pS("Go echo", TRUE);
            interrupt(0x21, 0xFF << 8 | 0x6, "echoUtil", 0x2000, &succ);
        }
        else if (type == cd)
        {
        }
        else if (type == ls)
        {
        }
        else if (type == mkdir)
        {
        }
        else if (type == rm)
        {
        }
        else if (type == cat)
        {
        }
        else if (type == runLocal)
        {
        }
        else if (type == runGlobal)
        {
        }
    }
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