public class MediumTire extends Tire {

  public MediumTire() {
    this.speed = 310;
    this.degradation = 0;
  }

  public void tick(TrackFeature f) {
    double featureTypeMultiplier = 0;

    if (f.getClass().getName().equals("HighSpeedTurn"))
      featureTypeMultiplier = 1.55;
    if (f.getClass().getName().equals("LowSpeedTurn"))
      featureTypeMultiplier = 1.3;
    if (f.getClass().getName().equals("Straight"))
      featureTypeMultiplier = 1;

    this.degradation += featureTypeMultiplier * f.roughness * 1.1;

    if (this.speed > 100)
      this.speed -= Math.min(75, this.degradation) * 0.25;
  }
}
