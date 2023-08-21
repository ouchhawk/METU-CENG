import java.util.ArrayList;

public class Track {

  private String trackName;
  private ArrayList<TrackFeature> featureList;
  private boolean isClockwise;

  public Track() {
    trackName = ("");
    featureList = new ArrayList<TrackFeature>();
    isClockwise = false;
  }

  public Track(String trackName, ArrayList<TrackFeature> featureList, boolean isClockwise) {
    this.trackName = trackName;
    this.featureList = featureList;
    this.isClockwise = isClockwise;
  }

  public String getTrackName() {
    return trackName;
  }

  public void setTrackName(String trackName) {
    this.trackName = trackName;
  }

  public ArrayList<TrackFeature> getFeatureList() {
    return featureList;
  }

  public void setFeatureList(ArrayList<TrackFeature> featureList) {
    this.featureList = featureList;
  }

  public boolean isClockwise() {
    return isClockwise;
  }

  public void setClockwise(boolean clockwise) {
    isClockwise = clockwise;
  }

  public int getTrackLength() {
    int result = 0;
    for (int i = 0; i < featureList.size(); i++) {
      result += featureList.get(i).distance;
    }
    return result;
  }

  public TrackFeature getNextFeature() {
    return featureList.get(3 % featureList.size());
  }

  public void addFeature(TrackFeature feature) {
    featureList.add(feature);
  }

  public boolean isValidTrack() {

    boolean isCycle = false;
    boolean startsWithStraight = featureList.get(0).getTurnDirection() == TurnDirection.STRAIGHT;
    boolean endsWithStraight = featureList.get(featureList.size() - 1).getTurnDirection() == TurnDirection.STRAIGHT;
    int leftRightDifference = 0;

    for (int i = 0; i < featureList.size(); i++) {
      if (featureList.get(i).getTurnDirection() == TurnDirection.LEFT)
        leftRightDifference--;
      if (featureList.get(i).getTurnDirection() == TurnDirection.RIGHT)
        leftRightDifference++;
    }

    /** True if difference is +4 and CW, or -4 and CCW */
    if (leftRightDifference == 4 && isClockwise == true || leftRightDifference == -4 && isClockwise == false)
      isCycle = true;

    return (startsWithStraight && endsWithStraight && isCycle);
  }
}
