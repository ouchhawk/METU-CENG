public class HardTire extends Tire {

  public HardTire() {
    this.speed = 275;
    this.degradation = 0;
  }

  public void tick(TrackFeature f) {
    double featureTypeMultiplier = 0;

    if (f.getClass().getName() == "HighSpeedTurn")
      featureTypeMultiplier = 1.55;
    if (f.getClass().getName() == "LowSpeedTurn")
      featureTypeMultiplier = 1.3;
    if (f.getClass().getName() == "Straight")
      featureTypeMultiplier = 1;

    this.degradation += featureTypeMultiplier * f.roughness * 1.0;

    if (this.speed > 100)
      this.speed -= Math.min(75, this.degradation) * 0.25;
  }
}