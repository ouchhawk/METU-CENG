import java.util.ArrayList;

public class Session {

  private Track track;
  private ArrayList<Team> teamList;
  private int totalLaps;

  public Session() {
    this.track = new Track();
    this.teamList = null;
    this.totalLaps = 0;
  }

  public Session(Track track, ArrayList<Team> teamList, int totalLaps) {
    this.track = track;
    this.teamList = new ArrayList<Team>(teamList);
    this.totalLaps = totalLaps;
  }

  public Track getTrack() {
    return track;
  }

  public void setTrack(Track track) {
    this.track = track;
  }

  public ArrayList<Team> getTeamList() {
    return teamList;
  }

  public void setTeamList(ArrayList<Team> teamList) {
    this.teamList = teamList;
  }

  public int getTotalLaps() {
    return totalLaps;
  }

  public void setTotalLaps(int totalLaps) {
    this.totalLaps = totalLaps;
  }

  public void simulate() {

    if (track.isValidTrack() == false) {
      System.out.println("Track is invalid.Simulation aborted!");
    } else {
      System.out
          .println("Track is valid.Strating simulation on" + track.getTrackName() + " for " + totalLaps + " laps.");

      for (int i = 0; i < track.getFeatureList().size(); i++) {
        for (int j = 0; j < teamList.size(); j++) {
          for (int k = 0; k < teamList.get(j).getCarList().size(); k++) {
            teamList.get(j).getCarList().get(k).getTire().tick(track.getFeatureList().get(i)); /** Call tire tick */
            teamList.get(j).getCarList().get(k).tick(track.getFeatureList().get(i)); /** Call car tick */
          }
        }
      }
      printWinnerTeam();
      printTimingTable();
    }
  }

  public ArrayList<Car> generateRankTable() {
    int indexOfMin = 0, carListSize=0;
    ArrayList<Car> carList = new ArrayList<Car>();
    ArrayList<Car> rankedCarList = new ArrayList<Car>();
    Car min = new Car();

    /** First, create "Car List" */
    for (int j = 0; j < teamList.size(); j++) {
      for (int k = 0; k < teamList.get(j).getCarList().size(); k++) {
        carList.add(teamList.get(j).getCarList().get(k));
      }
    }

    /**
     * Second, take the "Car List", find car with min totalTime each time, add to
     * "Ranked Car List" so that Cars become sorted
     */
    carListSize=carList.size();
    for (int i = 0; i < carListSize; i++) {
      min = carList.get(0);
      indexOfMin=0;
      for (int j = 0; j < carList.size(); j++) {
        if (carList.get(j).getTotalTime() < min.getTotalTime()) {
          min = carList.get(j);
          indexOfMin = j;
        }
      }
      rankedCarList.add(min);
      carList.remove(indexOfMin);
    }
    return rankedCarList;
  }

  public void printWinnerTeam() {
    int i = 0, len =0;
    Team winnerTeam = new Team();
    ArrayList<Car> finalTable = new ArrayList<>();
    finalTable = generateRankTable();

    /** Find winner's team */
    for (int j = 0; j < teamList.size(); j++) {
      for (int k = 0; k < teamList.get(j).getCarList().size(); k++) {
        if (teamList.get(j).getCarList().get(k).equals(finalTable.get(0))) {
          winnerTeam = teamList.get(j);
        }
      }
    }

    /** Printing part */
    System.out.print("Team " + winnerTeam.getName() + " wins.");
    len=winnerTeam.getTeamColors().length;
    for (i = 0; i < len - 1;i++) {
      if ((len-i) == 2) {
        System.out.print(winnerTeam.getTeamColors()[i] + " and ");
      } else {
        System.out.print(winnerTeam.getTeamColors()[i] + ", ");
      }
    }
    System.out.println(winnerTeam.getTeamColors()[i] + " flags are waving everywhere.");
  }

  private void printTimingTable() {
    int hrs = 0, mins = 0, secs = 0, ms = 0;
    ArrayList<Car> finalTable = new ArrayList<>();
    finalTable = generateRankTable();

    for (int i = 0; i < finalTable.size(); i++) {
      hrs = (int) (finalTable.get(i).getTotalTime() / 60);
      mins = (int) (finalTable.get(i).getTotalTime() % 60);
      secs = (int) ((finalTable.get(i).getTotalTime() * 60) % 60);
      ms = (int) ((finalTable.get(i).getTotalTime() * 60 * 60) % 60);

      System.out.println(finalTable.get(i).getDriverName() + "(" + finalTable.get(i).getCarNo() + ")  " + hrs + ":"
          + mins + ":" + secs + "." + ms);
    }
  }
}
