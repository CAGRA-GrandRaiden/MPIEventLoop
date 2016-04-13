// http://www.rosshemsley.co.uk/2011/02/creating-a-progress-bar-in-c-or-any-other-console-app/                          
// Process has done i out of n rounds, and we want a bar of width w and resolution r.   
static inline void loadBar(Long64_t x, Long64_t n, Int_t resolution, Int_t width)
{
  // Only update r times.                                                                                               
  if (x % (n / resolution) != 0) return;
  // Calculuate the ratio of complete-to-incomplete.                                                                    
  Float_t ratio = x / (Float_t)n;
  Int_t   c     = ratio * width;
  // Show the percentage complete.                                                                                      
  std::cout << setw(5) << setiosflags(ios::fixed) << setprecision(1) << (Float_t)(ratio * 100.) << "% [";
  // Show the load bar.                                                                                                 
  for (Int_t i = 0; i < c;       i++) std::cout << "=";
  for (Int_t i = c; i < width - 1; i++) std::cout << " ";
  std::cout << "]";
  // dirty hack below                                                                                                   
  if (x == n - 1) {
    //    std::cout << std::endl << std::endl;                                                                          
  } else {
    std::cout << "\r" << std::flush;
  }
}
