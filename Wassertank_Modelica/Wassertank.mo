model Wassertank
  // Eingangsgrößen
  input Real u_in;           // Eingangsfluss (z.B. Wasserzuflussrate)
  input Real u_out;          // Ausgangsfluss (z.B. Wasserabflussrate)
  
  // Ausgangsgröße
  output Real y;             // Aktueller Wasserstand im Tank

  // Ausgangssignale
  output Boolean y_low;      // Ausgangssignal, wenn der Wasserstand niedrig ist
  output Boolean y_high;     // Ausgangssignal, wenn der Wasserstand hoch ist
  
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

protected 
  discrete Boolean lowSignal; // Hilfsvariable für y_low
  discrete Boolean highSignal; // Hilfsvariable für y_high

equation
  // Berechnung der Änderung der Höhe basierend auf Eingangs- und Ausgangsfluss
  limIntegrator.u = u_in - u_out;

  // Ausgabe der aktuellen Höhe des Wasserstands
  y = limIntegrator.y;

  // Setzen der Hilfssignale basierend auf der aktuellen Höhe des Wasserstands
  when {y <= eps, y >= (max_hoehe - eps), not (y <= eps) and not (y >= (max_hoehe - eps))} then
    if y <= eps then
      lowSignal = true;
      highSignal = false;
    elseif y >= (max_hoehe - eps) then
      lowSignal = false;
      highSignal = true;
    else
      lowSignal = false;
      highSignal = false;
    end if;
  end when;

  // Zuweisung der Ausgangssignale
  y_low = lowSignal;
  y_high = highSignal;

end Wassertank;