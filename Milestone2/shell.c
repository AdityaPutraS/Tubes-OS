#define SECTOR_SIZE 512
#define MAP_SECTOR 0x100     //Diubah agar sesuai spek
#define DIRS_SECTOR 0x101    //Diubah agar sesuai spek
#define FILES_SECTOR 0x102   //Ditambah agar sesuai spek
#define SECTORS_SECTOR 0x103 //Ditambah agar sesuai spek
#define MAX_DIRSNAME 15      //Ditambah agar sesuai spek
#define MAX_DIRS 32
#define TRUE 1
#define FALSE 0
#define EMPTY 0x00

/*
    KOLOM CHAT
    Todo list dong...
    Urg coba cd yak
    ok
    
    

    TODO :
    - cat (-w buat write)
    - cd
    - run lokal (cth : ./keyproc)
    - run global (cth : keyproc) <- keyproc harus ada di root

    Q&A

    HOW TO USE:
    1. pS, pI, pC
        seperti namanya, print string, print integer, print char
        nerima 2 parameter = yang mau di print dan apakah ada newline/tidak
        ada newline/tidak pake TRUE/FALSE

    2. interrupt
        sama kaya di spek
    
    3. cara compile
        run aja ./CompileOS.sh
        terus cek di terminal, ada error ga, kalo ga ada teken c terus enter

    4. argc -> banyakya argumen vektor
       argv -> argumen vektornya
       bener ga dit? ._.
       bener fidh
       argv[0] = commandnya
       echo Hai ITB
       argc = 2
       argv = {"echo", "Hai", "ITB"}
argumen vectorng diurusnyadimana ?

    BUG:
    1. Copy kadang bermasalah, kadang mau di suatu fungsi, kadang ngga
    2. rm kadang kurang cepet ngapusnya, jadi lebih cepet keluar success drpd dia ngapus
        jadinya filenya ga keapus tapi success
    3. ls kalo 
*/

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
void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex);
void search(char *sector, char awal, char sisanya[15], char *index, char *success);

int main()
{
    int b1, b2, i, j, idx, num;
    char isExit;
    char curDir;
    char inputSeparator[1];
    char input[100], temp[100], fileName[15];
    int inputLen, splitLen;
    int type, i, copyStart, copyEnd;
    char argc, succ, ada;
    char argv[64][128];
    char dirs[SECTOR_SIZE];
    char file[SECTOR_SIZE];
    int konstantaRun;
    char same;
    char success;

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
        split(input, 0x20, argv); // <- get argumen vektor
        if (type == echo)
        {
            pS("Command echo", TRUE);
            interrupt(0x21, 0x20, curDir, argc, argv + 1);
            interrupt(0x21, curDir << 8 | 0x6, "echoUtil", 0x2000, &succ);
        }
        else if (type == cd)
        {
            pS("Command cd", TRUE);
            if (argc > 1)
            {
                pS("Jumlah parameter cd harus 1 atau tidak sama sekali", TRUE);
            }
            else if (argc == 0)
            {
                curDir = 0xFF;
                //Asyique
            }
            else if (argc == 1)
            {
                //KODE CD ADA DI SHELL BIAR GA DIHAPUS SAMA RM
                //cek parameternya .. atau nama folder
                if (isSame(argv[1], "..")) // btw fidh, ini ada kemungkinan ngebug, di urg kadang mau kadang ngga
                {
                    //Ambil dirs pake read sector
                    //Cari parent dari index ke (curDir) di dirs
                    //setArgs curdir jadi itu
                    //run shell dengan args yang baru
                    if (curDir != 0xFF)
                    { //root
                        pS("Go up 1 level", TRUE);
                        interrupt(0x21, (0 << 8) | 0x02, dirs, DIRS_SECTOR, 0);
                        curDir = dirs[curDir * 16]; //parent dari curDir
                    }
                }
                else
                {
                    pS("Go to : ", FALSE);
                    pS(argv[1], TRUE);
                    //Ambil dirs pake read sector
                    //Cari index dari folder yang namanya argv[1] dan parentnya curdir
                    //setArgs curdir jadi index folder itu
                    //run shell dengan args yang baru
                    interrupt(0x21, (0 << 8) | 0x02, dirs, DIRS_SECTOR, 0);
                    searchDir(dirs, argv[1], &idx, &success, curDir);
                    if (success)
                    {
                        curDir = idx;
                    }
                    //hmmm, seems legit
                    //11/10
                    //AWKAWKWKWKWKWK
                    //ehiya ada yang kurang
                    // kalo cd doang gapake argumen langsung ke root ga sih?
                    // iya juga ya
                }
            }
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
            if (argc != 1 || argc != 2)
            {
                pS("cat menerima 1 / 2 parameter", TRUE);
            }
            else
            {
                interrupt(0x21, 0x20, curDir, argc, argv + 1);
                interrupt(0x21, curDir << 8 | 0x6, "cat", 0x2000, &succ);
            }
        }
        else if (type == runLocal || type == runGlobal)
        {
            if (type == runLocal)
            {
                pS("Command runLocal", TRUE);
                konstantaRun = 2;
            }
            else
            {
                pS("Command runGlobal", TRUE);
                konstantaRun = 0;
            }
            interrupt(0x21, 0x02, file, FILES_SECTOR, 0);
            //Cek dulu apakah filenya ada
            ada = FALSE;
            for (i = 0; (i < SECTOR_SIZE) && !ada; i++)
            {
                if (file[i] == curDir)
                {
                    clear(fileName, 15);
                    for (j = 0; j < 15; j++)
                    {
                        fileName[j] = file[i + j + 1];
                    }
                    ada = isSame(argv[0] + konstantaRun, fileName);
                }
            }
            if (ada)
            {
                //argv[0] = nama file yang mau di run (mulai dari index ke 2)
                //sisanya = parameternya

                //persiapan args
                interrupt(0x21, 0x20, curDir, argc, argv + 1);
                //run programnya
                interrupt(0x21, curDir << 8 | 0x6, argv[0] + konstantaRun, 0x2000, &succ);
            }
            else
            {
                pS(argv[0] + konstantaRun, FALSE);
                pS(" not found", TRUE);
            }
        }
    }
}

void search(char *sector, char awal, char sisanya[15], char *index, char *success)
{
    int i, j;
    char copied[15];
    clear(copied, 15);
    *success = FALSE;
    *index = 0;
    // printString("Masuk search , cari : ", FALSE);
    // printString(sisanya, TRUE);
    for (i = 0; (i < 32) && !(*success); i++)
    {
        if (sector[i * 16] == awal)
        {
            //Kopi sebagian dr sektor
            clear(copied, 15);
            for (j = 0; j < 15; j++)
            {
                copied[j] = sector[i * 16 + j + 1];
            }
            // printInt(i, TRUE);
            // printString("V", TRUE);
            // printString(copied, TRUE);
            *success = isSame(copied, sisanya);
        }
    }
    // printString("SELESAI", TRUE);
    *index = i - 1;
}

void searchDir(char *dirs, char *relPath, char *index, char *success, char parentIndex)
{
    int i, pathLength, countSlash;
    char pathSplitted[MAX_DIRS][MAX_DIRSNAME];
    char cari[MAX_DIRSNAME];
    if (relPath[0] == '\0')
    {
        *success = TRUE;
        *index = parentIndex;
    }
    else
    {
        //Split path
        len(relPath, &pathLength);
        count(relPath, '/', &countSlash);
        split(relPath, '/', pathSplitted);
        // printString("isi 0 : ", FALSE);
        // printString(pathSplitted[0], TRUE);
        // printString("isi 1 : ", FALSE);
        // printString(pathSplitted[1], TRUE);
        // printString("count slash : ", FALSE);
        // printInt(countSlash, TRUE);
        *success = TRUE;
        for (i = 0; (i < (countSlash + 1)) && *success; i++)
        {
            // printString("pathSplitted skrng : ", FALSE);
            // printString(pathSplitted[i], TRUE);
            clear(cari, 15);
            copy(pathSplitted[i], cari, 0, 15);
            search(dirs, parentIndex, cari, index, success);
            if (!success)
            {
                // printString("BANGSAT", TRUE);
            }
            parentIndex = *index;
        }
    }
}
//urg debugging dulu kalo gitu
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