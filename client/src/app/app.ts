import { ChangeDetectionStrategy, Component, signal } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { Authentication } from '@components/authentication/authentication'

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, Authentication],
  templateUrl: './app.html',
  styleUrl: './app.scss',
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class App {
  protected readonly title = signal('devola');
}
