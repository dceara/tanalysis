int main()
{
  int* buf;
  buf [10] = taint();
  return 0;
}
