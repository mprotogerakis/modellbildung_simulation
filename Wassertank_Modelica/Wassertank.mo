model Wassertank
  // Eingangsgrößen
  input Real u_in;           // Eingangsfluss (z.B. Wasserzuflussrate)
  input Boolean y_low;       // Ausgangssignal, wenn der Wasserstand niedrig ist
  input Boolean y_high;      // Ausgangssignal, wenn der Wasserstand hoch ist
  input Real u_out;          // Ausgangsfluss (z.B. Wasserabflussrate)
  
  // Ausgangsgröße
  output Real y;             // Aktueller Wasserstand im Tank

  // Parameter
  parameter Real max_hoehe = 1;           // Maximale Höhe des Wassertanks
  parameter Real initial_hoehe = 0.5;     // Anfangshöhe des Wasserstands im Tank
  constant Real eps = 1e-3;               // Kleiner Wert für die Toleranz (Epsilon)

  // Kontinuierlicher, begrenzter Integrator aus der Modelica-Bibliothek
  Modelica.Blocks.Continuous.LimIntegrator limIntegrator(
    outMax = max_hoehe,       // Maximale Ausgangshöhe des Integrators
    outMin = 0,               // Minimale Ausgangshöhe des Integrators
    y_start = initial_hoehe   // Startwert des Integrators (initialer Wasserstand)
  );

equation
  // Berechnung der Änderung der Höhe basierend auf Eingangs- und Ausgangsfluss
  limIntegrator.u = u_in - u_out;

  // Ausgabe der aktuellen Höhe des Wasserstands
  y = limIntegrator.y;

  // Setzen der Ausgangssignale basierend auf der aktuellen Höhe des Wasserstands
  if y <= eps then
    y_low = true;         // Wenn der Wasserstand sehr niedrig ist, setze y_low auf true
    y_high = false;       // und y_high auf false
  elseif y >= (max_hoehe - eps) then
    y_low = false;        // Wenn der Wasserstand sehr hoch ist, setze y_low auf false
    y_high = true;        // und y_high auf true
  else
    y_low = false;        // Ansonsten setze beide Ausgangssignale auf false
    y_high = false;
  end if;

end Wassertank;