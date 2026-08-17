import { Component, effect } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { ThemeService } from '@services/theme/theme';

@Component({
    selector: 'app-theme',
    imports: [MatIconModule],
    templateUrl: './theme.html',
    styleUrl: './theme.scss',
})
export class ThemeComponent {

    constructor(private themeService: ThemeService) {
        effect(this.themeService.saveTheme.bind(this.themeService));
    }

    get theme() {
        return this.themeService.theme;
    }

    toggleTheme(): void {
        this.themeService.toggle();
    }
}
