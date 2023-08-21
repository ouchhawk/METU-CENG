public class LowSpeedTurn extends TrackFeature {

  public LowSpeedTurn(int turnNo, TurnDirection direction, double distance, double roughness) {
    super.featureNo = turnNo;
    super.turnDirection = direction;
    super.distance = distance;
    super.roughness = roughness;
  }
}
