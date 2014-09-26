# Autor: Michal Lukac, xlukac05@stud.fit.vutbr.cz, 1.3.2011
# Info : Projekt do predmetu IVS na VUT FIT. Ulohou tohto 
#        skriptu je vypocet kvadratickej rovnice.

# Funkcia solve riesi kvadraticku rovnicu.
# a,b,c - cleny kvadratickej funkcie.
def solve(a, b, c)
  if a == 0 and b == 0
    return nil
  elsif a == 0 
    return [-c.to_f/b]
  end

  # Vypocet diskriminantu
  d = b.to_f**2 - 4*a*c
  if d == 0
    return [-b.to_f/(2*a)]
  elsif d > 0
    x1= (-b.to_f+d**(1/2))/(2*a)
    x2= (-b.to_f-d**(1/2))/(2*a)
    return [x1, x2]
  end
  return nil
end
