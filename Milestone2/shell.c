int main()
{
    char input[100];
    interrupt(0x21, (0 << 8) | 0, "$ ", 0, 0);
    interrupt(0x21, (0 << 8) | 1, &input, 0, 0);
    interrupt(0x21, (0 << 8) | 0, input, 1, 0);
}