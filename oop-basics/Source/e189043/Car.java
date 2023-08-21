public class Car {

  private int carNo;
  private String driverName;
  private double totalTime;
  private Tire tire;

  public Car() {
    this.carNo = 0;
    this.driverName = "";
    this.tire = null;
    this.totalTime = 0;
  }

  public Car(String driverName, int carNo, Tire tire) {
    this.carNo = carNo;
    this.driverName = driverName;
    this.tire = tire;
    this.totalTime = 0;
  }

  public Tire getTire() {
    return tire;
  }

  public void setTire(Tire tire) {
    this.tire = tire;
  }

  public String getDriverName() {
    return driverName;
  }

  public void setDriverName(String driverName) {
    this.driverName = driverName;
  }

  public int getCarNo() {
    return carNo;
  }

  public void setCarNo(int carNo) {
    this.carNo = carNo;
  }

  public double getTotalTime() {
    return totalTime;
  }

  public void setTotalTime(double total) {
    this.totalTime = total;
  }

  public void tick(TrackFeature feature) {

    /** Calculate time spent */
    double timeSpent = feature.getDistance() / tire.getSpeed() + Math.random();
    this.totalTime += timeSpent;

    /** Change tires if too much degradation */
    if (tire.getDegradation() > 70) {
      if (tire.getClass().getName() == "SoftTire")
        tire = new MediumTire();
      if (tire.getClass().getName() == "MediumTire")
        tire = new SoftTire();
      if (tire.getClass().getName() == "HardTire")
        tire = new SoftTire();
      this.totalTime += 25;

    }

  }

}
