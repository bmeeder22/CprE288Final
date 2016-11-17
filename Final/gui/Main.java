import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class Main {
    private JFrame mainFrame;
    private JLabel statusLabel;
    private JPanel arrowpad;
    private JPanel optionPanel;
    private ListClient client;

    public Main(){
        prepareGUI();
    }

    public static void main(String[] args){
        Main swingControlDemo = new Main();
        swingControlDemo.renderOptionPanel();
        swingControlDemo.renderArrowpad();

        System.out.println("done!");
    }

    private void prepareGUI(){
        client = new ListClient();
        mainFrame = new JFrame("Java SWING Examples");
        mainFrame.setSize(500,500);
        mainFrame.setLayout(new GridLayout(3, 1));

        statusLabel = new JLabel("",JLabel.CENTER);

        statusLabel.setSize(500,500);
        mainFrame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent windowEvent){
                System.exit(0);
            }
        });
        arrowpad = new JPanel();
        optionPanel = new JPanel();
        arrowpad.setLayout(new GridLayout(3,3,1,1));
        optionPanel.setLayout(new GridLayout(3,3,1,1));


        mainFrame.add(arrowpad);
        mainFrame.add(optionPanel);
        mainFrame.add(statusLabel);
        mainFrame.setVisible(true);
    }

    private void renderOptionPanel() {
        JButton button1 = new JButton("Sweep");
        JButton button2 = new JButton("2");
        JButton button3 = new JButton("3");
        JButton button4 = new JButton("4");

        button1.setActionCommand("sweep");
        button2.setActionCommand("2");
        button3.setActionCommand("3");
        button4.setActionCommand("4");

        button1.addActionListener(new ButtonClickListener());
        button2.addActionListener(new ButtonClickListener());
        button3.addActionListener(new ButtonClickListener());
        button4.addActionListener(new ButtonClickListener());

        optionPanel.add(button1);
        optionPanel.add(button2);
        optionPanel.add(button3);
        optionPanel.add(button4);
    }

    private void renderArrowpad(){
        JButton forwardButton = new JButton("Forward");
        JButton backButton = new JButton("Backwards");
        JButton leftButton = new JButton("Left");
        JButton rightButton = new JButton("Right");

        forwardButton.setActionCommand("Forward");
        backButton.setActionCommand("Backwards");
        leftButton.setActionCommand("Left");
        rightButton.setActionCommand("Right");

        forwardButton.addActionListener(new ButtonClickListener());
        backButton.addActionListener(new ButtonClickListener());
        leftButton.addActionListener(new ButtonClickListener());
        rightButton.addActionListener(new ButtonClickListener());

//        Row1
        arrowpad.add(new JLabel());
        arrowpad.add(forwardButton);
        arrowpad.add(new JLabel());

//        Row2
        arrowpad.add(leftButton);
        arrowpad.add(new JLabel());
        arrowpad.add(rightButton);

//        Row3
        arrowpad.add(new JLabel());
        arrowpad.add(backButton);
        arrowpad.add(new JLabel());

        mainFrame.setVisible(true);
    }

    private class ButtonClickListener implements ActionListener{
        public void actionPerformed(ActionEvent e) {
            String command = e.getActionCommand();

            switch(command) {
                case "Forward":
                    handleForward();
                    break;
                case "Backwards":
                    handleBack();
                    break;
                case "Left":
                    handleLeft();
                    break;
                case "Right":
                    handleRight();
                    break;
                case "sweep":
                    handleSweep();
                default:
                    statusLabel.setText("Button");
            }
        }

        private void handleRight() {
            statusLabel.setText("Right");
            client.sendMessage("r");
        }

        private  void handleLeft() {
            statusLabel.setText("Left");
            client.sendMessage("l");
        }

        private void handleForward() {
            statusLabel.setText("Forward");
            client.sendMessage("f");
        }

        private void handleBack() {
            statusLabel.setText("Backwards");
            client.sendMessage("b");
        }

        private void handleSweep() {
            statusLabel.setText("Sweep");
            client.sendMessage("s");
        }
    }
}
